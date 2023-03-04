[EntityEditorProps(category: "GameScripted/CTI", description: "Structure placing")]
class SCR_CTI_PlacingStructureComponentClass: ScriptComponentClass
{
};

class SCR_CTI_PlacingStructureComponent : ScriptComponent
{
	protected SCR_PlayerController m_PlayerController;
	protected RplComponent m_RplComponent;
	
	protected FactionKey m_fk;
	protected ResourceName m_resName;
	
	//protected IEntity m_structure = null;
	SCR_BasePreviewEntity m_structure = null;
	
	protected vector finalMat[4];
	
	protected bool m_startPlacing = false;
	protected bool m_confirmed = false;
	protected bool m_canceled = false;
	
	protected float m_dist;
	protected int m_placement;
	
	protected float m_timeDelta;
	protected const float TIMESTEP = 0.01;

	//------------------------------------------------------------------------------------------------
	bool getStartPlacing()
	{
		return m_startPlacing;
	}

	//------------------------------------------------------------------------------------------------
	void confirmPlacement(bool value)
	{
		m_confirmed = value;
	}
	
	//------------------------------------------------------------------------------------------------
	void cancelPlacement(bool value)
	{
		m_canceled = value;
	}
	
	
	//------------------------------------------------------------------------------------------------
	void createStructurePreview(FactionKey fk, ResourceName resName, float distance, int placement, bool startPlacing)
	{
		m_fk = fk;
		m_resName = resName;
		m_dist = distance;
		m_placement = placement;
		
		vector mat[4];
		vector dir;
		
		IEntity m_player = m_PlayerController.GetControlledEntity();
		m_player.GetTransform(mat);
		dir = m_player.GetWorldTransformAxis(2);
		
		//PlayerCamera playerCam = GetGame().GetPlayerController().GetPlayerCamera();
		//playerCam.GetTransform(mat);

		//CharacterHeadAimingComponent chac = CharacterHeadAimingComponent.Cast(m_player.FindComponent(CharacterHeadAimingComponent));
		//dir = chac.GetAimingDirectionWorld();

		vector angles = mat[2].VectorToAngles();
		angles[0] = angles[0] + placement;
		mat[2] = angles.AnglesToVector();

		mat[3] = mat[3] + (dir * m_dist);
		mat[3][1] = GetGame().GetWorld().GetSurfaceY(mat[3][0], mat[3][2]);
		
		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform = mat;
		
		Resource resource = Resource.Load(resName);

		//m_structure = GetGame().SpawnEntityPrefabLocal(resource, GetGame().GetWorld(), params);
		
		ResourceName material = "{58F07022C12D0CF5}Assets/Editor/PlacingPreview/Preview.emat";
		m_structure = SCR_PrefabPreviewEntity.SpawnPreviewFromPrefab(resource, "SCR_PrefabPreviewEntity", GetGame().GetWorld(), params, material);

		if (m_structure)
		{
			//Physics phys = m_structure.GetPhysics();
			//phys.ChangeSimulationState(SimulationState.NONE);

			PrintFormat("CTI :: Created Structure preview: %1", m_structure);

			m_canceled = false;
			m_confirmed = false;
			m_startPlacing = startPlacing;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	protected void updatePreiew()
	{
		vector mat[4];
		vector dir;
		
		IEntity m_player = m_PlayerController.GetControlledEntity();
		m_player.GetTransform(mat);
		dir = m_player.GetWorldTransformAxis(2);
		
		//PlayerCamera playerCam = GetGame().GetPlayerController().GetPlayerCamera();
		//playerCam.GetTransform(mat);

		//CharacterHeadAimingComponent chac = CharacterHeadAimingComponent.Cast(m_player.FindComponent(CharacterHeadAimingComponent));
		//dir = chac.GetAimingDirectionWorld();

		vector angles = mat[2].VectorToAngles();
		angles[0] = angles[0] + m_placement;
		mat[2] = angles.AnglesToVector();

		mat[3] = mat[3] + (dir * m_dist);
		mat[3][1] = GetGame().GetWorld().GetSurfaceY(mat[3][0], mat[3][2]);
		
		//m_structure.SetTransform(mat);
		SetHierarchyTransform(m_structure, mat);
		m_structure.Update();

		finalMat = mat;
	}
	
	//------------------------------------------------------------------------------------------------
	protected void performBuilding()
	{
		auto menuManager = GetGame().GetMenuManager();
		menuManager.CloseAllMenus();

		m_startPlacing = false;
		
		SCR_EntityHelper.DeleteEntityAndChildren(m_structure);
		
		SCR_CTI_NetWorkComponent netComp = SCR_CTI_NetWorkComponent.Cast(m_PlayerController.FindComponent(SCR_CTI_NetWorkComponent));
		netComp.buildStructureServer(m_fk, m_resName, finalMat);

		m_canceled = false;
		m_confirmed = false;
	}

	//------------------------------------------------------------------------------------------------
	protected void cancelBuilding()
	{
		auto menuManager = GetGame().GetMenuManager();
		menuManager.CloseAllMenus();
		
		m_startPlacing = false;
		m_confirmed = false;
		m_canceled = false;

		if (m_structure) SCR_EntityHelper.DeleteEntityAndChildren(m_structure);
	}

	//------------------------------------------------------------------------------------------------
	//SCR_EntityHelper's function without dynamic physics test
	void SetHierarchyTransform(notnull IEntity ent, vector newTransform[4])
	{
		vector oldTransform[4];
		ent.GetTransform(oldTransform);
		ent.SetTransform(newTransform);

		IEntity child = ent.GetChildren();
		while (child)
		{
			SetHierarchyChildTransform(child, oldTransform, newTransform, true);
			child = child.GetSibling();
		}
	}

	//------------------------------------------------------------------------------------------------
	protected static void SetHierarchyChildTransform(notnull IEntity ent, vector oldTransform[4], vector newTransform[4], bool recursive = true)
	{
		vector mat[4];
		ent.GetTransform(mat);

		vector diffMat[4];
		Math3D.MatrixInvMultiply4(oldTransform, mat, diffMat);
		Math3D.MatrixMultiply4(newTransform, diffMat, mat);

		ent.SetTransform(mat);

		IEntity child = ent.GetChildren();
		while (child)
		{
			SetHierarchyChildTransform(child, oldTransform, newTransform, recursive);
			child = child.GetSibling();
		}
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsProxy()
	{
		return (m_RplComponent && m_RplComponent.IsProxy());
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		m_PlayerController = SCR_PlayerController.Cast(PlayerController.Cast(owner));
		
		if (!m_PlayerController)
		{
			Print("SCR_CTI_PlacingStructureComponent must be attached to PlayerController!", LogLevel.ERROR);
			return;
		}
		
		m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.FIXEDFRAME);
		owner.SetFlags(EntityFlags.ACTIVE, true);
		
		m_timeDelta = 0;
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnFixedFrame(IEntity owner, float timeSlice)
	{
		if (m_startPlacing)
		{
			m_timeDelta += timeSlice;
			if (m_timeDelta > TIMESTEP)
			{
				updatePreiew();
				if (m_confirmed) performBuilding();
				if (m_canceled) cancelBuilding();
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	void SCR_CTI_PlacingStructureComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
	}

	//------------------------------------------------------------------------------------------------
	void ~SCR_CTI_PlacingStructureComponent()
	{
	}
};