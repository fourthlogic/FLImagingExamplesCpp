#include <cstdio>
#include <cmath>
#include <array>
#include <vector>
#include <functional>
#include <sys/stat.h>
#include <FLImaging.h>
#include "../CommonHeader/ErrorPrint.h"

namespace SpacePlanning = FLImaging::ThreeDim::SpacePlanning;

// SpacePlanningDynamicSP 고급 예제: Destination 옆의 작은 Buffer 공간을 임시 적치에 사용합니다.
// Advanced SpacePlanningDynamicSP example: use a small Buffer area next to Destination as temporary storage.
// 일부 물품은 XZ 평면 회전, 일부 물품은 모든 축 회전을 허용합니다.
// Some items allow XZ-plane rotation, and one item allows full axis rotation.

namespace
{
	const int32_t i32BinDestination = 0;
	const int32_t i32BinBuffer = 1;
	const int32_t i32BinCount = 2;

	const float f32BinWorldSpacingX = 18.f;

	const float f32Pi = 3.14159265358979f;
	const float f32AnimationArcHeight = 4.f;
	const double f64AnimationDurationMs = 480.;
	const int32_t i32AnimationSleepMs = 1;
	const float f32SourcePreviewMaxTiltDegree = 20.f;
	const uint64_t u64SourceItemTypeRandomSeed = 0x9d84a3d390df0c46ull;
	const uint64_t u64SourceRotationRandomSeed = 0x6a09e667f3bcc909ull;

	enum ESourceState
	{
		ESourceState_NeedNewSource,
		ESourceState_HasSource,
	};

	struct SSourceSlot
	{
		ESourceState eState;
		int32_t i32ItemType;
		int32_t i32ArrivalIndex;
		CFLGeometry3DQuaternion<float> quatLocalRotation;
	};

	struct SRotationBasis
	{
		Base::TPoint3<float> tpAxisX;
		Base::TPoint3<float> tpAxisY;
		Base::TPoint3<float> tpAxisZ;
	};

	struct SBinFrame
	{
		Base::TPoint3<float> tpWorldPivot;
		Base::TPoint3<float> tpBinPivot;
		SRotationBasis basis;
	};

	struct SAnimationPose
	{
		Base::TPoint3<float> tpWorldCenter;
		CFLGeometry3DQuaternion<float> quatRotation;
	};

	struct SRuntimeModelSpecs
	{
		std::array<SpacePlanning::SBinSpec<float>, i32BinCount> arrBinSpecs;
		std::vector<SpacePlanning::SItemSpec<float>> vctItemSpecs;
	};

	typedef std::function<void(int32_t, const SAnimationPose&, const SAnimationPose&)> FnAnimateMove;

	double Clamp01(double f64Value)
	{
		if(f64Value <= 0.)
			return 0;
		if(f64Value >= 1.)
			return 1;

		return f64Value;
	}

	Base::TPoint3<float> Add(const Base::TPoint3<float>& tpA, const Base::TPoint3<float>& tpB)
	{
		return Base::TPoint3<float>(tpA.x + tpB.x, tpA.y + tpB.y, tpA.z + tpB.z);
	}

	Base::TPoint3<float> Sub(const Base::TPoint3<float>& tpA, const Base::TPoint3<float>& tpB)
	{
		return Base::TPoint3<float>(tpA.x - tpB.x, tpA.y - tpB.y, tpA.z - tpB.z);
	}

	Base::TPoint3<float> Scale(const Base::TPoint3<float>& tpValue, float f32Scale)
	{
		return Base::TPoint3<float>(tpValue.x * f32Scale, tpValue.y * f32Scale, tpValue.z * f32Scale);
	}

	float Dot(const Base::TPoint3<float>& tpA, const Base::TPoint3<float>& tpB)
	{
		return tpA.x * tpB.x + tpA.y * tpB.y + tpA.z * tpB.z;
	}

	Base::TPoint3<float> GetBinWorldOrigin(int32_t i32BinIndex)
	{
		return Base::TPoint3<float>(f32BinWorldSpacingX * i32BinIndex, 0.f, 0.f);
	}

	Base::TPoint3<float> Lerp(const Base::TPoint3<float>& tpStart, const Base::TPoint3<float>& tpEnd, float f32T)
	{
		return {tpStart.x + (tpEnd.x - tpStart.x) * f32T,tpStart.y + (tpEnd.y - tpStart.y) * f32T,tpStart.z + (tpEnd.z - tpStart.z) * f32T };
	}

	SRotationBasis MakeRotationBasis(const Base::TPoint3<float>& tpAxisX, const Base::TPoint3<float>& tpAxisY, const Base::TPoint3<float>& tpAxisZ)
	{
		SRotationBasis basis = {};
		basis.tpAxisX = tpAxisX;
		basis.tpAxisY = tpAxisY;
		basis.tpAxisZ = tpAxisZ;
		return basis;
	}

	CFLGeometry3DQuaternion<float> MakeQuaternionFromBasis(const SRotationBasis& basis)
	{
		CMatrixFor3D<float> matRotation(
			basis.tpAxisX.x, basis.tpAxisY.x, basis.tpAxisZ.x,
			basis.tpAxisX.y, basis.tpAxisY.y, basis.tpAxisZ.y,
			basis.tpAxisX.z, basis.tpAxisY.z, basis.tpAxisZ.z);

		CFLGeometry3DQuaternion<float> quat;
		quat.SetMatrix(matRotation);
		quat.Normalize();
		return quat;
	}

	SRotationBasis MakeRotationBasisFromQuaternion(const CFLGeometry3DQuaternion<float>& quat)
	{
		CMatrixFor3D<float> matRotation;
		quat.GetMatrix(matRotation);

		return MakeRotationBasis(
			Base::TPoint3<float>(matRotation[0][0], matRotation[1][0], matRotation[2][0]),
			Base::TPoint3<float>(matRotation[0][1], matRotation[1][1], matRotation[2][1]),
			Base::TPoint3<float>(matRotation[0][2], matRotation[1][2], matRotation[2][2]));
	}

	Base::TPoint3<float> TransformBinDirection(const SRotationBasis& binBasis, const Base::TPoint3<float>& tpLocalDirection)
	{
		return Add(
			Add(Scale(binBasis.tpAxisX, tpLocalDirection.x), Scale(binBasis.tpAxisY, tpLocalDirection.y)),
			Scale(binBasis.tpAxisZ, tpLocalDirection.z));
	}

	Base::TPoint3<float> BinLocalDirectionFromWorld(const SRotationBasis& binBasis, const Base::TPoint3<float>& tpWorldDirection)
	{
		return Base::TPoint3<float>(
			Dot(tpWorldDirection, binBasis.tpAxisX),
			Dot(tpWorldDirection, binBasis.tpAxisY),
			Dot(tpWorldDirection, binBasis.tpAxisZ));
	}

	CFL3DCamera GetWorldCamera()
	{
		CFL3DCamera cam;
		cam.SetProjectionType(E3DCameraProjectionType_Perspective);
		cam.SetDirection(CFLPoint3<float>(0.f, -0.85f, -0.53f));
		cam.SetDirectionUp(CFLPoint3<float>(0.f, 0.53f, -0.85f));
		cam.SetPosition(CFLPoint3<float>(14, 55, 30));

		return cam;
	}

	CFLGeometry3DQuaternion<float> MakeBinFrameRotation(int32_t i32BinIndex)
	{
		CFLGeometry3DQuaternion<float> quat;
		if(i32BinIndex == i32BinDestination)
			quat.SetEulerAngles(EEulerSequence_Extrinsic_XYZ, 6.f, 8.f, 5.f);
		else
			quat.SetEulerAngles(EEulerSequence_Extrinsic_XYZ, -7.f, 5.f, -6.f);
		quat.Normalize();
		return quat;
	}

	SBinFrame GetBinFrame(int32_t i32BinIndex)
	{
		SBinFrame frame = {};
		frame.tpWorldPivot = GetBinWorldOrigin(i32BinIndex);
		frame.tpBinPivot = Base::TPoint3<float>(0.f, 0.f, 0.f);
		frame.basis = MakeRotationBasisFromQuaternion(MakeBinFrameRotation(i32BinIndex));
		return frame;
	}

	Base::TPoint3<float> WorldFromBinLocal(int32_t i32BinIndex, const Base::TPoint3<float>& tpLocal)
	{
		const SBinFrame frame = GetBinFrame(i32BinIndex);
		const Base::TPoint3<float> tpDelta = Sub(tpLocal, frame.tpBinPivot);
		return Add(frame.tpWorldPivot, TransformBinDirection(frame.basis, tpDelta));
	}

	Base::TPoint3<float> BinLocalFromWorld(int32_t i32BinIndex, const Base::TPoint3<float>& tpWorld)
	{
		const SBinFrame frame = GetBinFrame(i32BinIndex);
		const Base::TPoint3<float> tpDeltaWorld = Sub(tpWorld, frame.tpWorldPivot);
		return Add(frame.tpBinPivot, BinLocalDirectionFromWorld(frame.basis, tpDeltaWorld));
	}

	CFLGeometry3DQuaternion<float> MakeWorldRotationFromLocalRotation(int32_t i32BinIndex, const CFLGeometry3DQuaternion<float>& quatLocalRotation)
	{
		CFLGeometry3DQuaternion<float> quatWorldRotation = MakeBinFrameRotation(i32BinIndex) * quatLocalRotation;
		quatWorldRotation.Normalize();
		return quatWorldRotation;
	}

	CFLGeometry3DQuaternion<float> MakeQuaternionFromRotationVector(const Base::TPoint3<float>& tpRotationVector)
	{
		const float f32Angle = sqrtf(Dot(tpRotationVector, tpRotationVector));
		CFLGeometry3DQuaternion<float> quat;

		if(f32Angle <= 1.e-6f)
			quat.SetEulerAngles(EEulerSequence_Extrinsic_XYZ, 0.f, 0.f, 0.f);
		else
		{
			const float f32InvAngle = 1.f / f32Angle;
			quat.SetAxisAndAngle(
				CFLGeometry3DVector<float>(tpRotationVector.x * f32InvAngle, tpRotationVector.y * f32InvAngle, tpRotationVector.z * f32InvAngle),
				f32Angle);
		}
		quat.Normalize();
		return quat;
	}

	CFLGeometry3DQuaternion<float> DrawSourcePreviewLocalRotation(CXorshiroRandomGenerator& rng)
	{
		const float f32MaxTiltRadian = f32SourcePreviewMaxTiltDegree * f32Pi / 180.f;
		const float f32Tilt = sqrtf(rng.GenerateUniformRandomValueF32(0.f, 1.f)) * f32MaxTiltRadian;
		const float f32Azimuth = rng.GenerateUniformRandomValueF32(0.f, 2.f * f32Pi);
		const Base::TPoint3<float> tpRotationVector(f32Tilt * cosf(f32Azimuth), 0.f, f32Tilt * sinf(f32Azimuth));
		return MakeQuaternionFromRotationVector(tpRotationVector);
	}

	void ApplyTransform(const CMatrixFor3D<float>& matRotation, const Base::TPoint3<float>& tpOffset, const std::vector<Base::TPoint3<float>>& vctLocalVertices, CFLArray<TPoint3<float>>& flaVertices)
	{
		const float mat00 = matRotation[0][0], mat01 = matRotation[0][1], mat02 = matRotation[0][2];
		const float mat10 = matRotation[1][0], mat11 = matRotation[1][1], mat12 = matRotation[1][2];
		const float mat20 = matRotation[2][0], mat21 = matRotation[2][1], mat22 = matRotation[2][2];

		for(int64_t i = 0; i < flaVertices.GetCount(); ++i)
		{
			const Base::TPoint3<float>& tpLocal = vctLocalVertices[static_cast<size_t>(i)];
			flaVertices[i] = Base::TPoint3<float>(
				tpOffset.x + mat00 * tpLocal.x + mat01 * tpLocal.y + mat02 * tpLocal.z,
				tpOffset.y + mat10 * tpLocal.x + mat11 * tpLocal.y + mat12 * tpLocal.z,
				tpOffset.z + mat20 * tpLocal.x + mat21 * tpLocal.y + mat22 * tpLocal.z);
		}
	}

	Base::TPoint3<float> GetObjectExtents(const SpacePlanning::SItemSpec<float>& itemSpec, const CFLGeometry3DQuaternion<float>& quatRotation)
	{
		CMatrixFor3D<float> matRotation;
		quatRotation.GetMatrix(matRotation);

		const float f32HalfWidth = itemSpec.width * 0.5f;
		const float f32HalfHeight = itemSpec.height * 0.5f;
		const float f32HalfDepth = itemSpec.depth * 0.5f;

		return Base::TPoint3<float>(
			f32HalfWidth * fabsf(matRotation[0][0]) + f32HalfHeight * fabsf(matRotation[0][1]) + f32HalfDepth * fabsf(matRotation[0][2]),
			f32HalfWidth * fabsf(matRotation[1][0]) + f32HalfHeight * fabsf(matRotation[1][1]) + f32HalfDepth * fabsf(matRotation[1][2]),
			f32HalfWidth * fabsf(matRotation[2][0]) + f32HalfHeight * fabsf(matRotation[2][1]) + f32HalfDepth * fabsf(matRotation[2][2]));
	}

	SAnimationPose MakePoseFromBinLocalAabbMin(const SpacePlanning::SItemSpec<float>& itemSpec, int32_t i32BinIndex, const Base::TPoint3<float>& tpAabbMinBinLocal, const CFLGeometry3DQuaternion<float>& quatLocalRotation)
	{
		SAnimationPose pose;
		pose.quatRotation = MakeWorldRotationFromLocalRotation(i32BinIndex, quatLocalRotation);
		pose.tpWorldCenter = WorldFromBinLocal(i32BinIndex, Add(tpAabbMinBinLocal, GetObjectExtents(itemSpec, quatLocalRotation)));
		return pose;
	}

	CFLGeometry3DQuaternion<float> FindClosestEquivalentCuboidRotation(const CFLGeometry3DQuaternion<float>& quatStart, const CFLGeometry3DQuaternion<float>& quatTarget)
	{
		// A centered cuboid is invariant under a 180-degree rotation about any local principal axis.
		const CFLGeometry3DQuaternion<float> arrLocalSymmetries[4] = {
			MakeQuaternionFromRotationVector({ 0.f, 0.f, 0.f }),
			MakeQuaternionFromRotationVector({ f32Pi, 0.f, 0.f }),
			MakeQuaternionFromRotationVector({ 0.f, f32Pi, 0.f }),
			MakeQuaternionFromRotationVector({ 0.f, 0.f, f32Pi }),
		};

		CFLGeometry3DQuaternion<float> quatClosest = quatTarget;
		double f64ClosestDot = -1.;
		for(int32_t i = 0; i < 4; ++i)
		{
			CFLGeometry3DQuaternion<float> quatCandidate = quatTarget * arrLocalSymmetries[i];
			quatCandidate.Normalize();

			double f64Dot = quatStart.Dot(quatCandidate);
			if(f64Dot < 0.)
			{
				quatCandidate *= -1.f;
				f64Dot = -f64Dot;
			}

			if(f64Dot > f64ClosestDot)
			{
				quatClosest = quatCandidate;
				f64ClosestDot = f64Dot;
			}
		}

		return quatClosest;
	}

	SAnimationPose LerpArc(const SAnimationPose& start, const SAnimationPose& end, float f32T, float f32ArcHeight)
	{
		SAnimationPose pose;
		pose.tpWorldCenter = Lerp(start.tpWorldCenter, end.tpWorldCenter, f32T);
		pose.tpWorldCenter.y += f32ArcHeight * sinf(f32Pi * f32T);
		pose.quatRotation.SetSphericalLinearInterpolation(start.quatRotation, end.quatRotation, f32T);
		return pose;
	}

	struct SItemInstance
	{
		int32_t i32ItemSpecIndex;
		SpacePlanning::EAxisRotation eRotation;
		Base::TPoint3<float> tpMin;
		Base::TPoint3<float> tpMax;
	};

	Base::TPoint3<float> GetRotatedItemSize(const SpacePlanning::SItemSpec<float>& itemSpec, SpacePlanning::EAxisRotation eRotation)
	{
		switch(eRotation)
		{
		case SpacePlanning::EAxisRotation_XYZ:
			return Base::TPoint3<float>(itemSpec.width, itemSpec.height, itemSpec.depth);
		case SpacePlanning::EAxisRotation_ZYX:
			return Base::TPoint3<float>(itemSpec.depth, itemSpec.height, itemSpec.width);
		case SpacePlanning::EAxisRotation_XZY:
			return Base::TPoint3<float>(itemSpec.width, itemSpec.depth, itemSpec.height);
		case SpacePlanning::EAxisRotation_ZXY:
			return Base::TPoint3<float>(itemSpec.depth, itemSpec.width, itemSpec.height);
		case SpacePlanning::EAxisRotation_YXZ:
			return Base::TPoint3<float>(itemSpec.height, itemSpec.width, itemSpec.depth);
		case SpacePlanning::EAxisRotation_YZX:
			return Base::TPoint3<float>(itemSpec.height, itemSpec.depth, itemSpec.width);
		}

		return Base::TPoint3<float>(itemSpec.width, itemSpec.height, itemSpec.depth);
	}

	Base::TPoint3<float> GetSourcePreviewLocalPos(const SpacePlanning::SItemSpec<float>& itemSpec, const SpacePlanning::SBinSpec<float>& binSpecBuffer)
	{
		const Base::TPoint3<float> tpSize = GetRotatedItemSize(itemSpec, SpacePlanning::EAxisRotation_XYZ);
		return Base::TPoint3<float>(binSpecBuffer.width + 2.f, 0.f, (binSpecBuffer.depth - tpSize.z) * 0.5f);
	}

	SItemInstance MakeItemInstance(const std::vector<SpacePlanning::SItemSpec<float>>& vctItemSpecs, const SpacePlanning::SPlacementInfo& placement)
	{
		const Base::TPoint3<float> tpSize = GetRotatedItemSize(vctItemSpecs[placement.i32ItemIndex], placement.eRotation);

		SItemInstance instance;
		instance.i32ItemSpecIndex = placement.i32ItemIndex;
		instance.eRotation = placement.eRotation;
		instance.tpMin = placement.tpPosition;
		instance.tpMax = Base::TPoint3<float>(
			placement.tpPosition.x + tpSize.x,
			placement.tpPosition.y + tpSize.y,
			placement.tpPosition.z + tpSize.z);

		return instance;
	}

	bool IsBelow(const SItemInstance& lower, const SItemInstance& upper)
	{
		const bool bXOverlap = (lower.tpMin.x < upper.tpMax.x) && (upper.tpMin.x < lower.tpMax.x);
		const bool bZOverlap = (lower.tpMin.z < upper.tpMax.z) && (upper.tpMin.z < lower.tpMax.z);
		const bool bUpperIsAbove = upper.tpMin.y >= lower.tpMax.y - 0.001f;

		return bXOverlap && bZOverlap && bUpperIsAbove;
	}

	struct SBinState
	{
		std::vector<SItemInstance> vctItems;
		std::vector<int32_t> vctCountAbove;

		void AddInstance(const SItemInstance& instance)
		{
			for(size_t i = 0; i < vctItems.size(); ++i)
			{
				if(IsBelow(vctItems[i], instance))
					vctCountAbove[i]++;
			}

			vctItems.push_back(instance);
			vctCountAbove.push_back(0);
		}

		CResult GetFirstPickableIndexOfType(int32_t i32ItemSpecIndex, int32_t& i32PlacedIndex) const
		{
			for(size_t i = 0; i < vctItems.size(); ++i)
			{
				if(vctItems[i].i32ItemSpecIndex == i32ItemSpecIndex && vctCountAbove[i] == 0)
				{
					i32PlacedIndex = static_cast<int32_t>(i);
					return EResult_OK;
				}
			}

			i32PlacedIndex = -1;
			return EResult_DoesNotExist;
		}

		CResult RemovePickableAt(int32_t i32PlacedIndex)
		{
			if(i32PlacedIndex < 0 || i32PlacedIndex >= static_cast<int32_t>(vctItems.size()) || vctCountAbove[i32PlacedIndex] != 0)
				return EResult_DoesNotExist;

			const SItemInstance removed = vctItems[i32PlacedIndex];
			vctItems.erase(vctItems.begin() + i32PlacedIndex);
			vctCountAbove.erase(vctCountAbove.begin() + i32PlacedIndex);

			for(size_t i = 0; i < vctItems.size(); ++i)
			{
				if(IsBelow(vctItems[i], removed))
					vctCountAbove[i]--;
			}

			return EResult_OK;
		}
	};

	bool WouldCoverBufferItem(const SBinState& binBuffer, const SItemInstance& item)
	{
		for(size_t i = 0; i < binBuffer.vctItems.size(); ++i)
		{
			if(IsBelow(binBuffer.vctItems[i], item))
				return true;
		}

		return false;
	}

	SpacePlanning::SPlacementInfo MakePlacementInfo(int32_t i32BinIndex, const SItemInstance& item)
	{
		SpacePlanning::SPlacementInfo placement;
		placement.i32BinIndex = i32BinIndex;
		placement.i32ItemIndex = item.i32ItemSpecIndex;
		placement.eRotation = item.eRotation;
		placement.tpPosition = item.tpMin;
		return placement;
	}

	CResult InitializeCoordinateConverter(const CSpacePlanningBaseSP& alg, CSpacePlanningCoordinateConverterSP& converter)
	{
		CResult res = EResult_UnknownError;

		do
		{
			if((res = alg.GetCoordinateConverter(converter)).IsFail())
				break;

			for(int32_t i = 0; i < alg.GetBinSpecCount(); ++i)
			{
				const SBinFrame frame = GetBinFrame(i);

				if((res = converter.SetBinTransform(i, frame.tpWorldPivot, frame.tpBinPivot, frame.basis.tpAxisZ, frame.basis.tpAxisY)).IsFail())
					break;
			}

			if(res.IsFail())
				break;

			for(int32_t i = 0; i < alg.GetItemSpecCount(); ++i)
			{
				if((res = converter.SetItemPivotNormalized(i, Base::TPoint3<float>(0.5f, 0.5f, 0.5f))).IsFail())
					break;
			}

			if(res.IsFail())
				break;

			res = converter.Learn();
		}
		while(false);

		return res;
	}

	void InitializeDefaultSourceItemChances(CFLArray<float>& itemChances)
	{
		itemChances.Resize(4);
		itemChances[0] = 4.f;
		itemChances[1] = 3.f;
		itemChances[2] = 3.f;
		itemChances[3] = 2.f;
	}

	CFLString<wchar_t> GetExecutablePath()
	{
		wchar_t wcsPath[MAX_PATH];
		wcsPath[0] = L'\0';
		GetModuleFileNameW(NULL, wcsPath, MAX_PATH);
		return CFLString<wchar_t>(wcsPath);
	}

	bool IsCacheUpToDate(const CFLString<wchar_t>& flsCache, const CFLString<wchar_t>& flsReference)
	{
		struct _stat64 stCache;
		if(_wstat64(flsCache.GetString(), &stCache) != 0)
			return false;

		struct _stat64 stReference;
		if(_wstat64(flsReference.GetString(), &stReference) != 0)
			return false;

		return stCache.st_mtime > stReference.st_mtime;
	}

	CResult ConfigureAndLearnDefaultModel(CSpacePlanningDynamicSP& alg, const CFLArray<float>& itemChances)
	{
		CResult res = EResult_UnknownError;

		do
		{
			alg.Clear();

			const std::array<SpacePlanning::SBinSpec<float>, i32BinCount> arrDefaultBinSpecs = {{
				SpacePlanning::SBinSpec<float>{ 9.f, 12.f, 10.f },
				SpacePlanning::SBinSpec<float>{ 6.f, 5.f, 6.f },
			}};

			for(size_t i = 0; i < arrDefaultBinSpecs.size(); ++i)
			{
				if((res = alg.AddBinSpec(arrDefaultBinSpecs[i])).IsFail())
					break;
			}

			if(res.IsFail())
				break;

			const std::array<SpacePlanning::SItemSpec<float>, 4> arrDefaultItemSpecs = {{
				SpacePlanning::SItemSpec<float>{ 3.0f, 3.0f, 4.0f, 1.f, SpacePlanning::ERotationAllowance_VerticalAxisOnly },
				SpacePlanning::SItemSpec<float>{ 2.0f, 4.3f, 5.9f, 1.f, SpacePlanning::ERotationAllowance_VerticalAxisOnly },
				SpacePlanning::SItemSpec<float>{ 4.0f, 3.0f, 3.5f, 1.f, SpacePlanning::ERotationAllowance_VerticalAxisOnly },
				SpacePlanning::SItemSpec<float>{ 5.0f, 3.0f, 2.5f, 1.f, SpacePlanning::ERotationAllowance_FullRotation },
			}};

			for(size_t i = 0; i < arrDefaultItemSpecs.size(); ++i)
			{
				if((res = alg.AddItemSpec(arrDefaultItemSpecs[i])).IsFail())
					break;
			}

			if(res.IsFail())
				break;

			SpacePlanning::SRandomSequenceParameters params(itemChances, 2);
			if((res = alg.SetRandomSequenceParameters(params)).IsFail())
				break;

			res = alg.Learn();
		}
		while(false);

		return res;
	}

	const wchar_t* GetStrategyGroupName(SpacePlanning::EStrategyGroup eGroup)
	{
		switch(eGroup)
		{
		case SpacePlanning::EStrategyGroup_Online:
			return L"Online";
		case SpacePlanning::EStrategyGroup_Layered:
			return L"Layered";
		case SpacePlanning::EStrategyGroup_Search:
			return L"Search";
		}

		return L"?";
	}

	const wchar_t* GetStrategyName(const CSpacePlanningBaseSP& alg, SpacePlanning::SSpacePlanningStrategyId sStrategyId)
	{
		SpacePlanning::SStrategyInfo info = {};
		if(alg.GetStrategyInfo(sStrategyId, info).IsFail() || info.pWcsStrategyName == NULL)
			return L"?";

		return info.pWcsStrategyName;
	}

	CResult LearnOrLoadDefaultModel(CSpacePlanningDynamicSP& alg, CFLArray<float>& itemChances,
	                                const CFLString<wchar_t>& flsCache, const CFLString<wchar_t>& flsSource)
	{
		CResult res = EResult_UnknownError;

		if(IsCacheUpToDate(flsCache, flsSource))
		{
			res = alg.Load(flsCache);

			if(res.IsOK() && alg.IsLearned())
			{
				SpacePlanning::SRandomSequenceParameters params;
				if((res = alg.GetRandomSequenceParameters(params)).IsFail())
					return res;

				itemChances = params.itemChances;

				const SpacePlanning::SSpacePlanningStrategyId sSelected = alg.GetSelectedStrategyId();
				wprintf(L"Loaded cached model: %s (strategy \"%s\" {%s, %d})\n",
					flsCache.GetString(), GetStrategyName(alg, sSelected),
					GetStrategyGroupName(sSelected.eGroup), sSelected.i32IDInStrategy);
				return res;
			}
		}

		InitializeDefaultSourceItemChances(itemChances);

		if((res = ConfigureAndLearnDefaultModel(alg, itemChances)).IsFail())
			return res;

		const SpacePlanning::SSpacePlanningStrategyId sOptimal = alg.GetOptimalStrategyId();
		if((res = alg.SelectStrategy(sOptimal)).IsFail())
			return res;

		const CResult resSave = alg.Save(flsCache);
		if(resSave.IsFail())
			wprintf(L"Warning: failed to cache model (%s): %s\n", flsCache.GetString(), resSave.GetString());
		else
			wprintf(L"Learned and cached model: %s (strategy \"%s\" {%s, %d})\n",
				flsCache.GetString(), GetStrategyName(alg, sOptimal),
				GetStrategyGroupName(sOptimal.eGroup), sOptimal.i32IDInStrategy);

		return res;
	}

	CResult LoadRuntimeSpecsFromModel(const CSpacePlanningBaseSP& alg, SRuntimeModelSpecs& modelSpecs)
	{
		CResult res = EResult_UnknownError;

		do
		{
			if(alg.GetBinSpecCount() < i32BinCount || alg.GetItemSpecCount() <= 0)
			{
				res = EResult_InvalidData;
				break;
			}

			for(int32_t i = 0; i < i32BinCount; ++i)
			{
				if((res = alg.GetBinSpec(i, modelSpecs.arrBinSpecs[i])).IsFail())
					break;
			}

			if(res.IsFail())
				break;

			modelSpecs.vctItemSpecs.clear();
			modelSpecs.vctItemSpecs.reserve(static_cast<size_t>(alg.GetItemSpecCount()));
			for(int32_t i = 0; i < alg.GetItemSpecCount(); ++i)
			{
				SpacePlanning::SItemSpec<float> itemSpec = {};
				if((res = alg.GetItemSpec(i, itemSpec)).IsFail())
					break;

				modelSpecs.vctItemSpecs.push_back(itemSpec);
			}

			if(res.IsFail())
				break;

			res = EResult_OK;
		}
		while(false);

		return res;
	}

	bool IsSameItemSpec(const SpacePlanning::SItemSpec<float>& lhs, const SpacePlanning::SItemSpec<float>& rhs)
	{
		return lhs.width == rhs.width &&
			lhs.height == rhs.height &&
			lhs.depth == rhs.depth &&
			lhs.load == rhs.load &&
			lhs.eAllowed == rhs.eAllowed;
	}

	CResult ValidateSameItemSpecs(const CSpacePlanningBaseSP& alg, const std::vector<SpacePlanning::SItemSpec<float>>& vctItemSpecs)
	{
		CResult res = EResult_UnknownError;

		do
		{
			if(alg.GetItemSpecCount() != static_cast<int32_t>(vctItemSpecs.size()))
			{
				res = EResult_Mismatched;
				break;
			}

			for(int32_t i = 0; i < alg.GetItemSpecCount(); ++i)
			{
				SpacePlanning::SItemSpec<float> itemSpec = {};
				if((res = alg.GetItemSpec(i, itemSpec)).IsFail())
					break;

				if(!IsSameItemSpec(itemSpec, vctItemSpecs[static_cast<size_t>(i)]))
				{
					res = EResult_Mismatched;
					break;
				}
			}

			if(res.IsFail())
				break;

			res = EResult_OK;
		}
		while(false);

		return res;
	}

	void BuildPlacementArray(const SBinState& bin, int32_t i32BinIndex, SpacePlanning::CFLPlacementInfoArray& flpaOut)
	{
		flpaOut.Clear();
		for(size_t i = 0; i < bin.vctItems.size(); ++i)
			flpaOut.PushBack(MakePlacementInfo(i32BinIndex, bin.vctItems[i]));
	}

	CResult PushBinToView(CGUIView3DWrap& view3D, CSpacePlanningCoordinateConverterSP& converter, CFL3DObjectGroup& flogBins, const SBinState& bin, int32_t i32BinIndex)
	{
		CResult res = EResult_UnknownError;

		do
		{
			int32_t i32BinObjIndex = 0;
			view3D.PushObject(*flogBins.GetObjectByIndex(i32BinIndex), &i32BinObjIndex);
			((CGUIView3DObject*)view3D.GetView3DObject(i32BinObjIndex))->SetOpacity(0.15f);

			if(bin.vctItems.empty())
			{
				res = EResult_OK;
				break;
			}

			SpacePlanning::CFLPlacementInfoArray flpaItems;
			BuildPlacementArray(bin, i32BinIndex, flpaItems);

			CFL3DObjectGroup flogItems;
			if((res = converter.MakeItemObjectGroup(flpaItems, flogItems)).IsFail())
				break;

			for(size_t i = 0; i < bin.vctItems.size(); ++i)
			{
				int32_t i32ObjIndex = 0;
				view3D.PushObject(*flogItems.GetObjectByIndex(static_cast<int32_t>(i)), &i32ObjIndex);
				((CGUIView3DObject*)view3D.GetView3DObject(i32ObjIndex))->SetOpacity(0.6f);
			}

			res = EResult_OK;
		}
		while(false);

		return res;
	}

	CResult CaptureObjectLocalVertices(CGUIView3DWrap& view3D, int32_t i32ObjIndex, const Base::TPoint3<float>& tpWorldCenter, std::vector<Base::TPoint3<float>>& vctLocalVertices)
	{
		CResult res = EResult_UnknownError;
		vctLocalVertices.clear();

		do
		{
			auto pViewObject = const_cast<CGUIView3DObject*>(view3D.GetView3DObject(i32ObjIndex));
			if(!pViewObject)
			{
				res = EResult_NullPointer;
				break;
			}

			auto pFlo = const_cast<CFL3DObject*>(pViewObject->Get3DObject());
			if(!pFlo)
			{
				res = EResult_NullPointer;
				break;
			}

			const auto& flaVertices = *pFlo->GetVertices();
			const int64_t i64VertexCount = flaVertices.GetCount();
			vctLocalVertices.reserve(static_cast<size_t>(i64VertexCount));

			for(int64_t i = 0; i < i64VertexCount; ++i)
				vctLocalVertices.push_back(Sub(flaVertices[i], tpWorldCenter));

			res = EResult_OK;
		}
		while(false);

		return res;
	}

	CResult UpdateItemObjectPose(CGUIView3DWrap& view3D, int32_t i32ObjIndex, const std::vector<Base::TPoint3<float>>& vctLocalVertices, const SAnimationPose& pose)
	{
		CResult res = EResult_UnknownError;

		do
		{
			auto pViewObject = const_cast<CGUIView3DObject*>(view3D.GetView3DObject(i32ObjIndex));
			if(!pViewObject)
			{
				res = EResult_NullPointer;
				break;
			}

			auto pFlo = const_cast<CFL3DObject*>(pViewObject->Get3DObject());
			if(!pFlo)
			{
				res = EResult_NullPointer;
				break;
			}

			auto& flaVertices = *const_cast<CFLArray<TPoint3<float>>*>(pFlo->GetVertices());
			const int64_t i64VertexCount = flaVertices.GetCount();
			if(i64VertexCount != static_cast<int64_t>(vctLocalVertices.size()))
			{
				res = EResult_Mismatched;
				break;
			}

			CMatrixFor3D<float> matRotation;
			pose.quatRotation.GetMatrix(matRotation);

			ApplyTransform(matRotation, pose.tpWorldCenter, vctLocalVertices, flaVertices);

			pViewObject->UpdateVertex();

			res = EResult_OK;
		}
		while(false);

		return res;
	}

	CResult PushItemObjectAtPose(CGUIView3DWrap& view3D, CSpacePlanningCoordinateConverterSP& converter, const SpacePlanning::SItemSpec<float>& itemSpec, int32_t i32ItemSpecIndex, int32_t i32RenderBinIndex, const SAnimationPose& pose, float f32Opacity, int32_t& i32ObjIndex, std::vector<Base::TPoint3<float>>* pVctLocalVertices)
	{
		CResult res = EResult_UnknownError;
		i32ObjIndex = -1;

		do
		{
			const Base::TPoint3<float> tpCenterBinLocal = BinLocalFromWorld(i32RenderBinIndex, pose.tpWorldCenter);
			const Base::TPoint3<float> tpUnrotatedMinBinLocal(
				tpCenterBinLocal.x - itemSpec.width * 0.5f,
				tpCenterBinLocal.y - itemSpec.height * 0.5f,
				tpCenterBinLocal.z - itemSpec.depth * 0.5f);

			SpacePlanning::SPlacementInfo placement;
			placement.i32BinIndex = i32RenderBinIndex;
			placement.i32ItemIndex = i32ItemSpecIndex;
			placement.eRotation = SpacePlanning::EAxisRotation_XYZ;
			placement.tpPosition = tpUnrotatedMinBinLocal;

			SpacePlanning::CFLPlacementInfoArray flpaItem;
			flpaItem.PushBack(placement);

			CFL3DObjectGroup flogItem;
			if((res = converter.MakeItemObjectGroup(flpaItem, flogItem)).IsFail())
				break;

			view3D.PushObject(*flogItem.GetObjectByIndex(0), &i32ObjIndex);
			((CGUIView3DObject*)view3D.GetView3DObject(i32ObjIndex))->SetOpacity(f32Opacity);

			std::vector<Base::TPoint3<float>> vctLocalVertices;
			if((res = CaptureObjectLocalVertices(view3D, i32ObjIndex, pose.tpWorldCenter, vctLocalVertices)).IsFail())
				break;

			const SRotationBasis renderBinBasis = GetBinFrame(i32RenderBinIndex).basis;
			for(size_t i = 0; i < vctLocalVertices.size(); ++i)
				vctLocalVertices[i] = BinLocalDirectionFromWorld(renderBinBasis, vctLocalVertices[i]);

			if((res = UpdateItemObjectPose(view3D, i32ObjIndex, vctLocalVertices, pose)).IsFail())
				break;

			view3D.UpdateObject(i32ObjIndex);

			if(pVctLocalVertices)
				*pVctLocalVertices = vctLocalVertices;

			res = EResult_OK;
		}
		while(false);

		return res;
	}

	CResult PushSourcePreviewToView(CGUIView3DWrap& view3D, CSpacePlanningCoordinateConverterSP& converter, const SpacePlanning::SItemSpec<float>& itemSpec, int32_t i32ItemSpecIndex, const SpacePlanning::SBinSpec<float>& binSpecBuffer, const CFLGeometry3DQuaternion<float>& quatLocalRotation)
	{
		const SAnimationPose poseSource = MakePoseFromBinLocalAabbMin(itemSpec, i32BinBuffer, GetSourcePreviewLocalPos(itemSpec, binSpecBuffer), quatLocalRotation);

		int32_t i32ObjIndex = -1;
		return PushItemObjectAtPose(view3D, converter, itemSpec, i32ItemSpecIndex, i32BinBuffer, poseSource, 0.85f, i32ObjIndex, nullptr);
	}

	CResult PushInFlightItemToView(CGUIView3DWrap& view3D, CSpacePlanningCoordinateConverterSP& converter, const SpacePlanning::SItemSpec<float>& itemSpec, int32_t i32ItemSpecIndex, const SAnimationPose& pose, int32_t& i32ObjIndex, std::vector<Base::TPoint3<float>>& vctLocalVertices)
	{
		return PushItemObjectAtPose(view3D, converter, itemSpec, i32ItemSpecIndex, i32BinDestination, pose, 0.95f, i32ObjIndex, &vctLocalVertices);
	}

	CResult RebuildInteractiveState(CSpacePlanningDynamicSP& alg, const std::array<SBinState, i32BinCount>& arrBins, int32_t i32ExcludedBinIndex, int32_t i32ExcludedPlacedIndex)
	{
		CResult res = EResult_UnknownError;

		do
		{
			if((res = alg.ClearInteractiveStates()).IsFail())
				break;

			if((res = alg.Execute()).IsFail())
				break;

			bool bFailed = false;
			for(int32_t i32BinIndex = 0; i32BinIndex < i32BinCount; ++i32BinIndex)
			{
				const SBinState& bin = arrBins[i32BinIndex];
				for(size_t i = 0; i < bin.vctItems.size(); ++i)
				{
					if(i32BinIndex == i32ExcludedBinIndex && static_cast<int32_t>(i) == i32ExcludedPlacedIndex)
						continue;

					const SpacePlanning::SPlacementInfo placement = MakePlacementInfo(i32BinIndex, bin.vctItems[i]);
					if((res = alg.PushItem(placement.i32ItemIndex)).IsFail() ||
					   (res = alg.AddPlacement(placement)).IsFail())
					{
						bFailed = true;
						break;
					}
				}

				if(bFailed)
					break;
			}

			if(bFailed)
				break;

			res = EResult_OK;
		}
		while(false);

		return res;
	}

	CResult FindRecommendedPlacementInBin(CSpacePlanningDynamicSP& alg, int32_t i32BinIndex, SpacePlanning::SPlacementInfo& placement)
	{
		CResult res = EResult_UnknownError;

		do
		{
			const int32_t i32CandidateCount = 256;
			SpacePlanning::CFLPlacementInfoArray flpaCandidates;
			if((res = alg.GetRecommendedNextPlacements(i32CandidateCount, flpaCandidates)).IsFail())
				break;

			for(int64_t i = 0; i < flpaCandidates.GetCount(); ++i)
			{
				if(flpaCandidates[i].i32BinIndex == i32BinIndex)
				{
					placement = flpaCandidates[i];
					return EResult_OK;
				}
			}

			res = EResult_DoesNotExist;
		}
		while(false);

		return res;
	}

	CResult TryPlaceSourceInBuffer(CSpacePlanningDynamicSP& alg, const CSpacePlanningCoordinateConverterSP& converter, std::array<SBinState, i32BinCount>& arrBins, const std::vector<SpacePlanning::SItemSpec<float>>& vctItemSpecs, const SpacePlanning::SBinSpec<float>& binSpecBuffer, SSourceSlot& sourceSlot, bool& bPlaced, const std::function<void()>& fnOnStep, const FnAnimateMove& fnAnimateMove)
	{
		CResult res = EResult_UnknownError;
		bPlaced = false;
		const int32_t i32SourceItemType = sourceSlot.i32ItemType;
		const int32_t i32ArrivalIndex = sourceSlot.i32ArrivalIndex;

		do
		{
			if((res = RebuildInteractiveState(alg, arrBins, -1, -1)).IsFail())
				break;

			if((res = alg.PushItem(i32SourceItemType)).IsFail())
				break;

			SpacePlanning::SPlacementInfo placement;
			res = FindRecommendedPlacementInBin(alg, i32BinBuffer, placement);
			if(res.IsFail())
			{
				if(res == EResult_DoesNotExist || res == EResult_FullOfCapacity)
				{
					wprintf(L"[source] arrival %2d: Buffer cannot accept item type %d.\n",
						i32ArrivalIndex, i32SourceItemType);
					res = EResult_OK;
				}

				break;
			}

			const SItemInstance item = MakeItemInstance(vctItemSpecs, placement);
			if(WouldCoverBufferItem(arrBins[i32BinBuffer], item))
			{
				wprintf(L"[source] arrival %2d: item type %d can be placed in Buffer, but would cover a buffered item.\n",
					i32ArrivalIndex, i32SourceItemType);
				res = EResult_OK;
				break;
			}

			SAnimationPose poseStart;
			SAnimationPose poseEnd;
			if(fnAnimateMove)
			{
				const SpacePlanning::SItemSpec<float>& itemSpec = vctItemSpecs[placement.i32ItemIndex];
				poseStart = MakePoseFromBinLocalAabbMin(itemSpec, i32BinBuffer, GetSourcePreviewLocalPos(itemSpec, binSpecBuffer), sourceSlot.quatLocalRotation);
				if((res = converter.ConvertPose(placement, poseEnd.tpWorldCenter, poseEnd.quatRotation)).IsFail())
					break;
			}

			if((res = alg.AddPlacement(placement)).IsFail())
				break;

			sourceSlot.eState = ESourceState_NeedNewSource;
			if(fnAnimateMove)
				fnAnimateMove(placement.i32ItemIndex, poseStart, poseEnd);

			arrBins[i32BinBuffer].AddInstance(item);

			wprintf(L"[source] arrival %2d: Source item type %d rotation %d -> Buffer [%.1f, %.1f, %.1f]  (Destination:%d, Buffer:%d)\n",
				i32ArrivalIndex, placement.i32ItemIndex, static_cast<int32_t>(placement.eRotation),
				placement.tpPosition.x, placement.tpPosition.y, placement.tpPosition.z,
				static_cast<int32_t>(arrBins[i32BinDestination].vctItems.size()),
				static_cast<int32_t>(arrBins[i32BinBuffer].vctItems.size()));

			if(fnOnStep)
				fnOnStep();

			bPlaced = true;
			res = EResult_OK;
		}
		while(false);

		return res;
	}

	CResult MoveOnePendingItemToDestination(CSpacePlanningDynamicSP& alg, const CSpacePlanningCoordinateConverterSP& converter, std::array<SBinState, i32BinCount>& arrBins, const std::vector<SpacePlanning::SItemSpec<float>>& vctItemSpecs, const SpacePlanning::SBinSpec<float>& binSpecBuffer, SSourceSlot& sourceSlot, const std::function<void()>& fnOnStep, const FnAnimateMove& fnAnimateMove)
	{
		CResult res = EResult_UnknownError;
		const int32_t i32SourceItemType = sourceSlot.i32ItemType;
		const int32_t i32ArrivalIndex = sourceSlot.i32ArrivalIndex;

		do
		{
			if((res = RebuildInteractiveState(alg, arrBins, -1, -1)).IsFail())
				break;

			bool bFailed = false;
			for(size_t i = 0; i < arrBins[i32BinBuffer].vctItems.size(); ++i)
			{
				if(arrBins[i32BinBuffer].vctCountAbove[i] != 0)
					continue;

				if((res = alg.PushItem(arrBins[i32BinBuffer].vctItems[i].i32ItemSpecIndex)).IsFail())
				{
					bFailed = true;
					break;
				}
			}

			if(bFailed)
				break;

			if((res = alg.PushItem(i32SourceItemType)).IsFail())
				break;

			SpacePlanning::SPlacementInfo placement;
			res = FindRecommendedPlacementInBin(alg, i32BinDestination, placement);
			if(res.IsFail())
			{
				if(res == EResult_DoesNotExist || res == EResult_FullOfCapacity)
					res = EResult_FullOfCapacity;
				break;
			}

			int32_t i32BufferPickIndex = -1;
			const bool bUseBufferedItem = arrBins[i32BinBuffer].GetFirstPickableIndexOfType(placement.i32ItemIndex, i32BufferPickIndex).IsOK();

			if(!bUseBufferedItem && placement.i32ItemIndex != i32SourceItemType)
			{
				res = EResult_DoesNotExist;
				break;
			}

			SAnimationPose poseStart;
			SAnimationPose poseEnd;
			if(fnAnimateMove)
			{
				if(bUseBufferedItem)
				{
					const SpacePlanning::SPlacementInfo placementStart = MakePlacementInfo(i32BinBuffer, arrBins[i32BinBuffer].vctItems[i32BufferPickIndex]);
					if((res = converter.ConvertPose(placementStart, poseStart.tpWorldCenter, poseStart.quatRotation)).IsFail())
						break;
				}
				else
				{
					const SpacePlanning::SItemSpec<float>& itemSpec = vctItemSpecs[placement.i32ItemIndex];
					poseStart = MakePoseFromBinLocalAabbMin(itemSpec, i32BinBuffer, GetSourcePreviewLocalPos(itemSpec, binSpecBuffer), sourceSlot.quatLocalRotation);
				}

				if((res = converter.ConvertPose(placement, poseEnd.tpWorldCenter, poseEnd.quatRotation)).IsFail())
					break;
			}

			if((res = alg.AddPlacement(placement)).IsFail())
				break;

			if(bUseBufferedItem)
			{
				if((res = arrBins[i32BinBuffer].RemovePickableAt(i32BufferPickIndex)).IsFail())
					break;
			}
			else
			{
				sourceSlot.eState = ESourceState_NeedNewSource;
			}

			if(fnAnimateMove)
				fnAnimateMove(placement.i32ItemIndex, poseStart, poseEnd);

			arrBins[i32BinDestination].AddInstance(MakeItemInstance(vctItemSpecs, placement));

			if((res = RebuildInteractiveState(alg, arrBins, -1, -1)).IsFail())
				break;

			wprintf(L"[destination] arrival %2d: %s item type %d rotation %d -> Destination [%.1f, %.1f, %.1f]  (Destination:%d, Buffer:%d)\n",
				i32ArrivalIndex, bUseBufferedItem ? L"Buffered" : L"Source",
				placement.i32ItemIndex, static_cast<int32_t>(placement.eRotation),
				placement.tpPosition.x, placement.tpPosition.y, placement.tpPosition.z,
				static_cast<int32_t>(arrBins[i32BinDestination].vctItems.size()),
				static_cast<int32_t>(arrBins[i32BinBuffer].vctItems.size()));

			if(fnOnStep)
				fnOnStep();

			res = EResult_OK;
		}
		while(false);

		return res;
	}

	CResult ProcessSourceArrival(CSpacePlanningDynamicSP& alg, const CSpacePlanningCoordinateConverterSP& converter, std::array<SBinState, i32BinCount>& arrBins, const std::vector<SpacePlanning::SItemSpec<float>>& vctItemSpecs, const SpacePlanning::SBinSpec<float>& binSpecBuffer, SSourceSlot& sourceSlot, const std::function<void()>& fnOnStep, const FnAnimateMove& fnAnimateMove)
	{
		CResult res = EResult_UnknownError;

		const int32_t i32MaxAttemptCount = static_cast<int32_t>(arrBins[i32BinBuffer].vctItems.size()) + 2;
		for(int32_t i32Attempt = 0; i32Attempt < i32MaxAttemptCount; ++i32Attempt)
		{
			bool bPlacedInBuffer = false;
			if((res = TryPlaceSourceInBuffer(alg, converter, arrBins, vctItemSpecs, binSpecBuffer, sourceSlot, bPlacedInBuffer, fnOnStep, fnAnimateMove)).IsFail())
				return res;

			if(bPlacedInBuffer)
				return EResult_OK;

			if((res = MoveOnePendingItemToDestination(alg, converter, arrBins, vctItemSpecs, binSpecBuffer, sourceSlot, fnOnStep, fnAnimateMove)).IsFail())
				return res;

			if(sourceSlot.eState == ESourceState_NeedNewSource)
				return EResult_OK;
		}

		return EResult_FullOfCapacity;
	}

	int32_t DrawSourceItemType(CXorshiroRandomGenerator& rng, const CFLArray<float>& itemChances, int32_t i32ItemSpecCount)
	{
		if(i32ItemSpecCount <= 0)
			return -1;

		if(itemChances.GetCount() == i32ItemSpecCount)
		{
			float f32TotalChance = 0.f;
			for(int64_t i = 0; i < itemChances.GetCount(); ++i)
				f32TotalChance += itemChances[i] > 0.f ? itemChances[i] : 0.f;

			if(f32TotalChance > 0.f)
			{
				const float f32Pick = rng.GenerateUniformRandomValueF32(0.f, f32TotalChance);
				float f32Accumulated = 0.f;
				for(int64_t i = 0; i < itemChances.GetCount(); ++i)
				{
					f32Accumulated += itemChances[i] > 0.f ? itemChances[i] : 0.f;
					if(f32Pick <= f32Accumulated)
						return static_cast<int32_t>(i);
				}

				return static_cast<int32_t>(itemChances.GetCount() - 1);
			}
		}

		return rng.GenerateUniformRandomValueI32(0, i32ItemSpecCount - 1);
	}
}

int main()
{
	// FLImaging(R) 라이브러리 사용 전 반드시 한 번 호출
	// Must be called once before using any FLImaging(R) features.
	CLibraryUtilities::Initialize();

	CGUIView3DWrap view3DResult;
	CResult res = EResult_UnknownError;

	do
	{
		CSpacePlanningDynamicSP alg;

		const CFLString<wchar_t> flsSource = GetExecutablePath();
		CFLString<wchar_t> flsCache;
		flsCache.Format(L"SpacePlanningDynamicIntermediateBuffer.%s", alg.GetFileExtension());

		CFLArray<float> itemChances;
		if((res = LearnOrLoadDefaultModel(alg, itemChances, flsCache, flsSource)).IsFail())
		{
			ErrorPrint(res, "Failed to prepare the default model.\n");
			break;
		}

		SRuntimeModelSpecs modelSpecs;
		if((res = LoadRuntimeSpecsFromModel(alg, modelSpecs)).IsFail())
		{
			ErrorPrint(res, "Failed to load runtime specs from the model.\n");
			break;
		}

		if((res = ValidateSameItemSpecs(alg, modelSpecs.vctItemSpecs)).IsFail())
		{
			ErrorPrint(res, "Loaded item specs do not match the runtime specs.\n");
			break;
		}

		CSpacePlanningCoordinateConverterSP converter;
		if((res = InitializeCoordinateConverter(alg, converter)).IsFail())
		{
			ErrorPrint(res, "Failed to initialize the coordinate converter.\n");
			break;
		}

		CFL3DObjectGroup flogBins;
		if((res = converter.MakeBinObjectGroup(flogBins)).IsFail())
		{
			ErrorPrint(res, "Failed to build world-space bin objects.\n");
			break;
		}

		if((res = view3DResult.Create(600, 0, 1300, 650)).IsFail())
		{
			ErrorPrint(res, "Failed to create the 3D view.\n");
			break;
		}

		view3DResult.SetRenderingTransparencyMode(ERenderingTransparencyMode_DepthPeelingOIT);
		view3DResult.SetRenderingResolutionScale(2);
		view3DResult.GetLayer(0).DrawTextCanvas(CFLPoint<double>(0, 0), L"Dynamic SP - Source / Buffer / Destination", YELLOW, BLACK, 20);

		std::array<SBinState, i32BinCount> arrBins;
		bool bZoomFitted = false;
		SSourceSlot sourceSlot;
		sourceSlot.eState = ESourceState_NeedNewSource;
		sourceSlot.i32ItemType = -1;
		sourceSlot.i32ArrivalIndex = 0;

		std::function<void()> fnDraw = [&]() -> void
		{
			if(!view3DResult.IsAvailable())
				return;

			view3DResult.Lock();
			view3DResult.ClearObjects();

			PushBinToView(view3DResult, converter, flogBins, arrBins[i32BinDestination], i32BinDestination);
			PushBinToView(view3DResult, converter, flogBins, arrBins[i32BinBuffer], i32BinBuffer);

			if(sourceSlot.eState == ESourceState_HasSource)
				PushSourcePreviewToView(view3DResult, converter, modelSpecs.vctItemSpecs[sourceSlot.i32ItemType], sourceSlot.i32ItemType, modelSpecs.arrBinSpecs[i32BinBuffer], sourceSlot.quatLocalRotation);

			CGUIView3DLayerWrap layer3DStatus = view3DResult.GetLayer(1);
			layer3DStatus.Clear();

			float f32TotalDestination = 0.f, f32UsedDestination = 0.f;
			float f32TotalBuffer = 0.f, f32UsedBuffer = 0.f;
			alg.GetCurrentVolumeUsage(i32BinDestination, f32TotalDestination, f32UsedDestination);
			alg.GetCurrentVolumeUsage(i32BinBuffer, f32TotalBuffer, f32UsedBuffer);

			CFLString<wchar_t> flsStatus;
			
			if(sourceSlot.eState == ESourceState_HasSource)
			{
				flsStatus.Format(L"Destination: %d items, %.1f / %.1f  |  Buffer: %d items, %.1f / %.1f  |  Source %d: item type %d",
					static_cast<int32_t>(arrBins[i32BinDestination].vctItems.size()), f32UsedDestination, f32TotalDestination,
					static_cast<int32_t>(arrBins[i32BinBuffer].vctItems.size()), f32UsedBuffer, f32TotalBuffer,
					sourceSlot.i32ArrivalIndex, sourceSlot.i32ItemType);
			}
			else
			{
				flsStatus.Format(L"Destination: %d items, %.1f / %.1f  |  Buffer: %d items, %.1f / %.1f",
					static_cast<int32_t>(arrBins[i32BinDestination].vctItems.size()), f32UsedDestination, f32TotalDestination,
					static_cast<int32_t>(arrBins[i32BinBuffer].vctItems.size()), f32UsedBuffer, f32TotalBuffer);
			}
			layer3DStatus.DrawTextCanvas(CFLPoint<double>(0, 25), flsStatus, YELLOW, BLACK, 16);

			if(!bZoomFitted && (sourceSlot.eState == ESourceState_HasSource || !arrBins[i32BinDestination].vctItems.empty() || !arrBins[i32BinBuffer].vctItems.empty()))
			{
				view3DResult.SetCamera(GetWorldCamera());
				bZoomFitted = true;
			}

			view3DResult.Unlock();
			view3DResult.Invalidate(true);
		};

		std::function<void()> fnRender = [&]() -> void
		{
			fnDraw();
			if(view3DResult.IsAvailable())
				CThreadUtilities::Sleep(500);
		};

		FnAnimateMove fnAnimateMove = [&](int32_t i32ItemType, const SAnimationPose& poseStart, const SAnimationPose& poseEnd) -> void
		{
			if(!view3DResult.IsAvailable())
				return;

			SAnimationPose poseEndMinimumMotion = poseEnd;
			poseEndMinimumMotion.quatRotation = FindClosestEquivalentCuboidRotation(poseStart.quatRotation, poseEnd.quatRotation);

			fnDraw();

			int32_t i32InFlightObjIndex = -1;
			std::vector<Base::TPoint3<float>> vctLocalVertices;
			view3DResult.Lock();
			const CResult resPush = PushInFlightItemToView(view3DResult, converter, modelSpecs.vctItemSpecs[i32ItemType], i32ItemType, poseStart, i32InFlightObjIndex, vctLocalVertices);
			view3DResult.Unlock();
			if(resPush.IsFail())
				return;
			view3DResult.Invalidate(true);

			CPerformanceCounter pcAnimation;
			pcAnimation.Start();

			while(view3DResult.IsAvailable())
			{
				const float f32T = static_cast<float>(Clamp01(pcAnimation.GetElapsedTimeFromStartInMilliSecond() / f64AnimationDurationMs));
				const SAnimationPose poseNext = LerpArc(poseStart, poseEndMinimumMotion, f32T, f32AnimationArcHeight);

				view3DResult.LockUpdate();
				const CResult resUpdate = UpdateItemObjectPose(view3DResult, i32InFlightObjIndex, vctLocalVertices, poseNext);
				view3DResult.UnlockUpdate();

				if(resUpdate.IsFail())
					break;

				view3DResult.UpdateObject(i32InFlightObjIndex);

				if(f32T >= 1.f)
					break;

				CThreadUtilities::Sleep(i32AnimationSleepMs);
			}
		};

		if((res = RebuildInteractiveState(alg, arrBins, -1, -1)).IsFail())
		{
			ErrorPrint(res, "Failed to initialize the interactive state.\n");
			break;
		}

		fnRender();

		CXorshiroRandomGenerator rngSourceItemType;
		rngSourceItemType.Seed(u64SourceItemTypeRandomSeed);
		CXorshiroRandomGenerator rngSourceRotation;
		rngSourceRotation.Seed(u64SourceRotationRandomSeed);

		int32_t i32NextArrivalIndex = 1;
		while(view3DResult.IsAvailable())
		{
			if(sourceSlot.eState == ESourceState_NeedNewSource)
			{
				sourceSlot.eState = ESourceState_HasSource;
				sourceSlot.i32ItemType = DrawSourceItemType(rngSourceItemType, itemChances, static_cast<int32_t>(modelSpecs.vctItemSpecs.size()));
				sourceSlot.i32ArrivalIndex = i32NextArrivalIndex;
				sourceSlot.quatLocalRotation = DrawSourcePreviewLocalRotation(rngSourceRotation);
				++i32NextArrivalIndex;

				fnRender();

				wprintf(L"[source] arrival %2d: item type %d\n", sourceSlot.i32ArrivalIndex, sourceSlot.i32ItemType);
			}

			if((res = ProcessSourceArrival(alg, converter, arrBins, modelSpecs.vctItemSpecs, modelSpecs.arrBinSpecs[i32BinBuffer], sourceSlot, fnRender, fnAnimateMove)).IsFail())
			{
				if(res == EResult_FullOfCapacity)
					wprintf(L"Arrival %d: Destination and Buffer cannot accept the source item. Stopping.\n", sourceSlot.i32ArrivalIndex);
				else if(view3DResult.IsAvailable())
					ErrorPrint(res, "Failed to process the source item.\n");

				break;
			}
		}

		wprintf(L"Dynamic intermediate buffer packing complete. Destination:%d, Buffer:%d\n",
			static_cast<int32_t>(arrBins[i32BinDestination].vctItems.size()),
			static_cast<int32_t>(arrBins[i32BinBuffer].vctItems.size()));

		while(view3DResult.IsAvailable())
			CThreadUtilities::Sleep(1);
	}
	while(false);

	return 0;
}
