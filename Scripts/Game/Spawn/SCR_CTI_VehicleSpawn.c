[EntityEditorProps(category: "GameScripted/CTI", description: "CTI Startup Vehicle Spawner")]
class SCR_CTI_VehicleSpawnClass : GenericEntityClass
{
};

class SCR_CTI_VehicleSpawn : GenericEntity
{
	protected Vehicle m_spawnedVehicle;

	//------------------------------------------------------------------------------------------------
	void spawnPrefab(ResourceName resName)
	{
		SCR_CTI_GameMode gameMode = SCR_CTI_GameMode.GetInstance();
		
		Resource resource = Resource.Load(resName);

		vector transform[4];
		GetTransform(transform);

		EntitySpawnParams params = new EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform = transform;

		IEntity newEnt = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		m_spawnedVehicle = Vehicle.Cast(newEnt);

		if (m_spawnedVehicle)
		{
			CarControllerComponent_SA carController = CarControllerComponent_SA.Cast(m_spawnedVehicle.FindComponent(CarControllerComponent_SA));
			if (carController) carController.SetPersistentHandBrake(true);
		
			Physics physicsComponent = m_spawnedVehicle.GetPhysics();
			if (physicsComponent) physicsComponent.SetVelocity("0 -1 0");

			SCR_ResourceContainer resContainer = FindVehicleResourceContainer(newEnt, EResourceType.SUPPLIES);
			if (resContainer) resContainer.SetResourceValue(0.0, false);

			ChimeraWorld world = newEnt.GetWorld();
			GarbageSystem garbageSystem = world.GetGarbageSystem();

			switch (resName)
			{
				case SCR_CTI_Constants.USSR_MHQ:
				{
					RplId rplId = Replication.FindId(newEnt);
					gameMode.setMHQrplId("USSR", rplId);

					garbageSystem.Insert(newEnt);
					garbageSystem.Withdraw(newEnt); // UpdateVictory Component handles MHQ lifetime

					IEntity child = newEnt.GetChildren();
					while (child)
					{
						SCR_SpawnPoint spawnPoint = SCR_SpawnPoint.Cast(child);
						if (spawnPoint)
						{
							spawnPoint.setDisplayName("USSR MHQ");
							break;
						}

						child = child.GetSibling();
					}

					break;
				}
				case SCR_CTI_Constants.US_MHQ:
				{
					RplId rplId = Replication.FindId(newEnt);
					gameMode.setMHQrplId("US", rplId);
	
					garbageSystem.Insert(newEnt);
					garbageSystem.Withdraw(newEnt); // UpdateVictory Component handles MHQ lifetime
	
					IEntity child = newEnt.GetChildren();
					while (child)
					{
						SCR_SpawnPoint spawnPoint = SCR_SpawnPoint.Cast(child);
						if (spawnPoint)
						{
							spawnPoint.setDisplayName("US MHQ");
							break;
						}
	
						child = child.GetSibling();
					}
	
					break;
				}
				default:
				{
					garbageSystem.Insert(newEnt, SCR_CTI_Constants.VEHICLECOLLECTIONTIME);
					garbageSystem.Withdraw(newEnt); // First GetIn event will start GM countdown timer
					//PrintFormat("CTI :: Default vehicle: %1", resName);
					break;
				}
			}
			//PrintFormat("CTI :: DEBUG :: Vehicle in garbage manager: %1 (%2)", garbageSystem.IsInserted(newEnt).ToString(), resName);
			//PrintFormat("CTI :: DEBUG :: Lifetime: %1 (%2)", garbageSystem.GetLifetime(newEnt), resName);
		} else {
			Print("CTI :: DEBUG :: VehicleSpawner did not created vehicle!");
		}
	}

	//------------------------------------------------------------------------------------------------
	void addItemsPrefab(map<ResourceName, int> prefabMap)
	{
		if (!m_spawnedVehicle) return;
		
		InventoryStorageManagerComponent ismc = InventoryStorageManagerComponent.Cast(m_spawnedVehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
		if (!ismc) return;
		
		UniversalInventoryStorageComponent uisc = UniversalInventoryStorageComponent.Cast(m_spawnedVehicle.FindComponent(UniversalInventoryStorageComponent));
		if (!uisc) return;

		foreach (ResourceName prefab, int piece : prefabMap)
		{
			for (int i = 0; i < piece; i++)
			{
				ismc.TrySpawnPrefabToStorage(prefab, uisc);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	// BI code from GarbageSystem
	protected SCR_ResourceContainer FindVehicleResourceContainer(notnull IEntity entity, EResourceType resourceType)
	{
		SCR_ResourceComponent resourceComponent = SCR_ResourceComponent.Cast(entity.FindComponent(SCR_ResourceComponent));
		if (!resourceComponent)
		{
			SlotManagerComponent slotManager = SlotManagerComponent.Cast(entity.FindComponent(SlotManagerComponent));
			if (!slotManager)
				return null;

			EntitySlotInfo slot = slotManager.GetSlotByName("Cargo");
			if (!slot)
				return null;

			entity = slot.GetAttachedEntity();
			if (!entity)
				return null;

			resourceComponent = SCR_ResourceComponent.Cast(entity.FindComponent(SCR_ResourceComponent));
		}

		if (!resourceComponent)
			return null;

		return resourceComponent.GetContainer(resourceType);
	}
};