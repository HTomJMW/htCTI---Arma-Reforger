[EntityEditorProps(category: "GameScripted/CTI", description: "Extinguish Vehicle - User Action")]
class SCR_CTI_ExtinguishAction : SCR_VehicleActionBase
{
	protected CarControllerComponent_SA m_pCarController;
	protected FactionAffiliationComponent m_vehAffiliationComp;
	protected SCR_CTI_GameMode m_gameMode;
	protected RplComponent m_rplComponent;

	protected bool hasExtinguisher = false;
	protected bool isFlaming = false;

	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_pCarController = CarControllerComponent_SA.Cast(pOwnerEntity.FindComponent(CarControllerComponent_SA));
		m_vehAffiliationComp = FactionAffiliationComponent.Cast(pOwnerEntity.FindComponent(FactionAffiliationComponent));
		m_gameMode = SCR_CTI_GameMode.Cast(GetGame().GetGameMode());
		m_rplComponent = RplComponent.Cast(pOwnerEntity.FindComponent(RplComponent));
	}

	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!hasExtinguisher)
		{
			SetCannotPerformReason("Extinguisher required");
			return false;
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------
	// Only on Server
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (m_rplComponent && m_rplComponent.IsProxy()) return;

		SCR_VehicleDamageManagerComponent vdmc = SCR_VehicleDamageManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_DamageManagerComponent));
		
		array<HitZone> outHitZones = {};
		vdmc.GetAllHitZones(outHitZones);
		
		foreach(HitZone hz : outHitZones)
		{
			SCR_FlammableHitZone part = SCR_FlammableHitZone.Cast(hz);
			
			if (!part) continue;
			if (part.GetFireState() != EFireState.NONE)
			{
				part.SetFireState(EFireState.NONE);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user);
		PlayerController pc = GetGame().GetPlayerManager().GetPlayerController(playerId);
		FactionAffiliationComponent affComp = FactionAffiliationComponent.Cast(pc.GetControlledEntity().FindComponent(FactionAffiliationComponent));
		IEntity veh = m_pCarController.GetOwner();
		VehicleWheeledSimulation_SA simulation = VehicleWheeledSimulation_SA.Cast(veh.FindComponent(VehicleWheeledSimulation_SA));
		SCR_VehicleDamageManagerComponent vdmc = SCR_VehicleDamageManagerComponent.Cast(veh.FindComponent(SCR_VehicleDamageManagerComponent));
		
		hasExtinguisher = false;
		SCR_PrefabNamePredicate predicate = new SCR_PrefabNamePredicate();
		array<IEntity> extinguishers = {};
		predicate.prefabName = SCR_CTI_Constants.EXTINGUISHER;
		SCR_InventoryStorageManagerComponent ismc;
		ismc = SCR_InventoryStorageManagerComponent.Cast(user.FindComponent(SCR_InventoryStorageManagerComponent));
		if (ismc) ismc.FindItems(extinguishers, predicate, EStoragePurpose.PURPOSE_ANY);
		if (!extinguishers.IsEmpty()) hasExtinguisher = true;

		isFlaming = false;
		array<HitZone> outHitZones = {};
		vdmc.GetAllHitZones(outHitZones);
		
		for (int i = outHitZones.Count() - 1; i >= 0; i--)
		{
			SCR_FlammableHitZone fhz = SCR_FlammableHitZone.Cast(outHitZones[i]);
			if (!fhz)
			{
				outHitZones.Remove(i);
			}
		}
		
		foreach(HitZone hz : outHitZones)
		{
			SCR_FlammableHitZone part = SCR_FlammableHitZone.Cast(hz);
			if (part.GetFireState() != EFireState.NONE)
			{
				isFlaming = true;
				break;
			}
		}

		if (Math.AbsFloat(simulation.GetSpeedKmh()) > 5) return false; // check vehicle speed less then 5
		if (!isFlaming) return false;

		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		outName = "[Hold] Extinguish";

		return true;
	}
};