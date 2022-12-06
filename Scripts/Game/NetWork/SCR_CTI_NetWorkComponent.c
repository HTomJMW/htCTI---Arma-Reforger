[EntityEditorProps(category: "GameScripted/CTI", description: "Handles client - server communication. Should be attached to PlayerController.")]
class SCR_CTI_NetWorkComponent : ScriptComponent
{
	protected SCR_PlayerController m_PlayerController;
	protected RplComponent m_RplComponent;

	//------------------------------------------------------------------------------------------------
	protected bool IsProxy()
	{
		return (m_RplComponent && m_RplComponent.IsProxy());
	}

	//------------------------------------------------------------------------------------------------
	void SendHint(int playerId, string message = "", string messageTitle = "", int hintTime = 5.0)
    {
        RpcAsk_RecieveHint(playerId, message, messageTitle, hintTime);
        Rpc(RpcAsk_RecieveHint, playerId, message, messageTitle, hintTime);
    }
   
	//------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcAsk_RecieveHint(int playerId, string message, string messageTitle, int hintTime)
    {
        int localPlayerId = GetGame().GetPlayerController().GetPlayerId();
        if(playerId != localPlayerId) return;
        
        SCR_HintManagerComponent.ShowCustomHint(message, messageTitle, hintTime);
    }

	//------------------------------------------------------------------------------------------------
	void SendPopUpNotif(int playerId, string message = "", float duration = 5.0, float fade = 0.5, string message2 = "", int prio = -1)
	{
		RpcAsk_RecievePopUpNotif(playerId, message, duration, fade, message2, prio);
        Rpc(RpcAsk_RecievePopUpNotif, playerId, message, duration, fade, message2, prio);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    protected void RpcAsk_RecievePopUpNotif(int playerId, string message, float duration, float fade, string message2, int prio)
    {
        int localPlayerId = GetGame().GetPlayerController().GetPlayerId();
        if(playerId != localPlayerId) return;

		SCR_PopUpNotification popUpNotif = SCR_PopUpNotification.GetInstance();
		popUpNotif.PopupMsg(message, duration, fade, message2);
    }

	//------------------------------------------------------------------------------------------------
	void unflipNearestVehicleServer(RplId vehRplId)
	{
		Rpc(RpcAsk_UnflipNearestVehicle, vehRplId);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_UnflipNearestVehicle(RplId vehRplId)
	{
		RplComponent rplComp = RplComponent.Cast(Replication.FindItem(vehRplId));
		IEntity vehicle = rplComp.GetEntity();

		Physics physics = vehicle.GetPhysics();
		
		vector vel = physics.GetVelocity();
		if (vel == "0 0 0")
		{
			physics.SetVelocity("0 4 0");

			vector angles = vehicle.GetAngles();
			if (angles[0] > 70 || angles[0] < -70) angles[0] = 0;
			if (angles[2] > 70 || angles[2] < -70) angles[2] = 0;
			vehicle.SetAngles(angles);
		}
	}

	//------------------------------------------------------------------------------------------------
	void StartUpgradeServer(FactionKey fk, int selected)
	{
		Rpc(RpcAsk_StartUpgrade, fk, selected);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_StartUpgrade(FactionKey fk, int selected)
	{
		SCR_CTI_UpgradeComponent upgradeComp = SCR_CTI_UpgradeComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_CTI_UpgradeComponent));
		upgradeComp.runUpgrade(fk, selected);
	}

	//------------------------------------------------------------------------------------------------
	void StopUpgradeServer(FactionKey fk)
	{
		Rpc(RpcAsk_StopUpgrade, fk);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_StopUpgrade(FactionKey fk)
	{
		SCR_CTI_UpgradeComponent upgradeComp = SCR_CTI_UpgradeComponent.Cast(GetGame().GetGameMode().FindComponent(SCR_CTI_UpgradeComponent));
		upgradeComp.stopUpgrade(fk);
	}

	//------------------------------------------------------------------------------------------------
	void setCommanderIdRpl(FactionKey fk, int playerId)
	{
		Rpc(RpcAsk_SetCommanderId, fk, playerId);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_SetCommanderId(FactionKey fk, int playerId)
	{
		SCR_CTI_GameMode gameMode = SCR_CTI_GameMode.Cast(GetGame().GetGameMode());
		gameMode.setCommanderId(fk, playerId);
	}

	//------------------------------------------------------------------------------------------------
	void clearCommanderIdRpl(FactionKey fk)
	{
		Rpc(RpcAsk_ClearCommanderId, fk);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_ClearCommanderId(FactionKey fk)
	{
		SCR_CTI_GameMode gameMode = SCR_CTI_GameMode.Cast(GetGame().GetGameMode());
		gameMode.clearCommanderId(fk);
	}
	
	//------------------------------------------------------------------------------------------------
	void buildDefenseServer(ResourceName resourcename, vector mat[4])
	{
		Rpc(RpcAsk_BuildDefenseServer, resourcename, mat);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_BuildDefenseServer(ResourceName resourcename, vector mat[4])
	{
		Resource resource = Resource.Load(resourcename);
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform = mat;

		GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
	}

	//------------------------------------------------------------------------------------------------
	void buildStructureServer(FactionKey factionkey, ResourceName resourcename, vector mat[4], float dist, float placement)
	{
		Rpc(RpcAsk_BuildStructureServer, factionkey, resourcename, mat, dist, placement);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_BuildStructureServer(FactionKey factionkey, ResourceName resourcename, vector mat[4], float dist, float placement)
	{
		SCR_CTI_BuildStructure BuildStructure = new SCR_CTI_BuildStructure;
		BuildStructure.build(factionkey, resourcename, mat, dist, placement);
	}
	
	//------------------------------------------------------------------------------------------------
	void factoryProductionServer(ResourceName resourcename, FactionKey factionkey, EntityID groupID, vector mat[4])
	{
		Rpc(RpcAsk_FactoryProductionServer, resourcename, factionkey, groupID, mat);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_FactoryProductionServer(ResourceName resourcename, FactionKey factionkey, EntityID groupID, vector mat[4])
	{
		SCR_CTI_FactoryProduction FactoryProduction = new SCR_CTI_FactoryProduction;
		FactoryProduction.build(resourcename, factionkey, groupID, mat);
	}
	
	//------------------------------------------------------------------------------------------------
	void transferResourcesServer(int senderId, int receiverId, int value)
	{
		Rpc(RpcAsk_TransferResourcesServer, senderId, receiverId, value);
	}

	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_TransferResourcesServer(int senderId, int receiverId, int value)
	{
		SCR_CTI_GameMode gameMode = SCR_CTI_GameMode.Cast(GetGame().GetGameMode());
		
		PlayerController pcSender = GetGame().GetPlayerManager().GetPlayerController(senderId);
		SCR_CTI_ClientDataComponent cdcSender = SCR_CTI_ClientDataComponent.Cast(pcSender.FindComponent(SCR_CTI_ClientDataComponent));
	
		cdcSender.changeFunds(-value);
		
		PlayerController pcReceiver = GetGame().GetPlayerManager().GetPlayerController(receiverId);
		SCR_CTI_ClientDataComponent cdcReceiver = SCR_CTI_ClientDataComponent.Cast(pcSender.FindComponent(SCR_CTI_ClientDataComponent));

		cdcReceiver.changeFunds(value);
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		m_PlayerController = SCR_PlayerController.Cast(PlayerController.Cast(owner));
		
		if (!m_PlayerController)
		{
			Print("SCR_CTI_NetworkComponent must be attached to PlayerController!", LogLevel.ERROR);
			return;
		}
		
		m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}

	//------------------------------------------------------------------------------------------------
	void SCR_CTI_NetWorkComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
	}

	//------------------------------------------------------------------------------------------------
	void ~SCR_CTI_NetWorkComponent()
	{
	}
};

class SCR_CTI_NetWorkComponentClass: ScriptComponentClass
{
};