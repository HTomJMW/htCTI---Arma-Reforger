[ComponentEditorProps(category: "GameScripted/CTI", description: "CTI Randomize start positions.")]
class SCR_CTI_RandomStartComponentClass : ScriptComponentClass
{
};

class SCR_CTI_RandomStartComponent : ScriptComponent
{
	protected bool m_ussrAtNorth = false;

	//------------------------------------------------------------------------------------------------	
	void randomStart()
	{
		RandomGenerator randomgen = new RandomGenerator();
		int h, m, s;
		System.GetHourMinuteSecond(h, m, s);
		randomgen.SetSeed(s * 100000);

		if (randomgen.RandIntInclusive(0, 1) == 0)
		{
			m_ussrAtNorth = true;
		} else {
			m_ussrAtNorth = false;
		}
		PrintFormat("CTI :: USSR Spawn at north: %1", m_ussrAtNorth.ToString());
		
		setupStart();
	}

	//------------------------------------------------------------------------------------------------
	protected void setupStart()
	{
		BaseWorld world = GetGame().GetWorld();

		SCR_CTI_VehicleSpawn svNorth1 = SCR_CTI_VehicleSpawn.Cast(world.FindEntityByName("SpawnVehicleNorth1"));
		SCR_CTI_VehicleSpawn svNorth2 = SCR_CTI_VehicleSpawn.Cast(world.FindEntityByName("SpawnVehicleNorth2"));
		SCR_CTI_VehicleSpawn svNorth3 = SCR_CTI_VehicleSpawn.Cast(world.FindEntityByName("SpawnVehicleNorth3"));
		
		SCR_CTI_VehicleSpawn svSouth1 = SCR_CTI_VehicleSpawn.Cast(world.FindEntityByName("SpawnVehicleSouth1"));
		SCR_CTI_VehicleSpawn svSouth2 = SCR_CTI_VehicleSpawn.Cast(world.FindEntityByName("SpawnVehicleSouth2"));
		SCR_CTI_VehicleSpawn svSouth3 = SCR_CTI_VehicleSpawn.Cast(world.FindEntityByName("SpawnVehicleSouth3"));

		// UAZ items
		map<ResourceName, int> itemMapUaz = new map<ResourceName, int>();
		itemMapUaz.Set(SCR_CTI_Constants.USSR_BANDAGE, 5);
		itemMapUaz.Set(SCR_CTI_Constants.USSR_AK_MAG, 10);
		itemMapUaz.Set(SCR_CTI_Constants.USSR_RPG7, 3);
		itemMapUaz.Set(SCR_CTI_Constants.USSR_RPG7_AMMO, 6);
		itemMapUaz.Set(SCR_CTI_Constants.USSR_GRENADE, 4);
		itemMapUaz.Set(SCR_CTI_Constants.TOOLKIT, 1);
		itemMapUaz.Set(SCR_CTI_Constants.EXTINGUISHER, 1);
		itemMapUaz.Set(SCR_CTI_Constants.USSR_BACKPACK, 2);

		// Ural items
		map<ResourceName, int> itemMapUral = new map<ResourceName, int>();
		itemMapUral.Set(SCR_CTI_Constants.USSR_BANDAGE, 5);
		itemMapUral.Set(SCR_CTI_Constants.USSR_AK_MAG, 10);
		itemMapUral.Set(SCR_CTI_Constants.USSR_RPG7, 3);
		itemMapUral.Set(SCR_CTI_Constants.USSR_RPG7_AMMO, 6);
		itemMapUral.Set(SCR_CTI_Constants.USSR_GRENADE, 4);
		itemMapUral.Set(SCR_CTI_Constants.TOOLKIT, 1);
		itemMapUral.Set(SCR_CTI_Constants.EXTINGUISHER, 1);
		itemMapUral.Set(SCR_CTI_Constants.USSR_BACKPACK, 2);
		
		// Ural MHQ items
		map<ResourceName, int> intemMapUralMHQ = new map<ResourceName, int>();
		intemMapUralMHQ.Set(SCR_CTI_Constants.USSR_BANDAGE, 5);
		intemMapUralMHQ.Set(SCR_CTI_Constants.TOOLKIT, 1);
		intemMapUralMHQ.Set(SCR_CTI_Constants.EXTINGUISHER, 1);
		intemMapUralMHQ.Set(SCR_CTI_Constants.USSR_BACKPACK, 2);
		
		// Jeep items
		map<ResourceName, int> itemMapM151 = new map<ResourceName, int>();
		itemMapM151.Set(SCR_CTI_Constants.US_BANDAGE, 5);
		itemMapM151.Set(SCR_CTI_Constants.US_M16_MAG, 10);
		itemMapM151.Set(SCR_CTI_Constants.US_M72, 9);
		itemMapM151.Set(SCR_CTI_Constants.US_GRENADE, 4);
		itemMapM151.Set(SCR_CTI_Constants.TOOLKIT, 1);
		itemMapM151.Set(SCR_CTI_Constants.EXTINGUISHER, 1);
		itemMapM151.Set(SCR_CTI_Constants.US_BACKPACK, 2);
		
		// M923 Items
		map<ResourceName, int> itemMapM923 = new map<ResourceName, int>();
		itemMapM923.Set(SCR_CTI_Constants.US_BANDAGE, 5);
		itemMapM923.Set(SCR_CTI_Constants.US_M16_MAG, 10);
		itemMapM923.Set(SCR_CTI_Constants.US_M72, 9);
		itemMapM923.Set(SCR_CTI_Constants.US_GRENADE, 4);
		itemMapM923.Set(SCR_CTI_Constants.TOOLKIT, 1);
		itemMapM923.Set(SCR_CTI_Constants.EXTINGUISHER, 1);
		itemMapM923.Set(SCR_CTI_Constants.US_BACKPACK, 2);
		
		// M923 MHQ items
		map<ResourceName, int> intemMapM923MHQ = new map<ResourceName, int>();
		intemMapM923MHQ.Set(SCR_CTI_Constants.US_BANDAGE, 5);
		intemMapM923MHQ.Set(SCR_CTI_Constants.TOOLKIT, 1);
		intemMapM923MHQ.Set(SCR_CTI_Constants.EXTINGUISHER, 1);
		intemMapM923MHQ.Set(SCR_CTI_Constants.US_BACKPACK, 2);
		
		if (m_ussrAtNorth)
		{
			// UAZ + equipment
			svNorth1.setPrefab(SCR_CTI_Constants.USSR_UAZ);
			svNorth1.addItemsPrefab(itemMapUaz);
			
			// Ural truck + equipment
			svNorth2.setPrefab("{3B4A2C1ED5DBB0B8}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_transport_covered.et");
			svNorth2.addItemsPrefab(itemMapUral);
			
			// USSR MHQ + equipment
			svNorth3.setPrefab(SCR_CTI_Constants.USSR_MHQ);
			svNorth3.addItemsPrefab(intemMapUralMHQ);
			
			// Jeep + equipment
			svSouth1.setPrefab(SCR_CTI_Constants.US_JEEP);
			svSouth1.addItemsPrefab(itemMapM151);
			
			// M923A1 truck + equipment
			svSouth2.setPrefab("{084A168713BD0D21}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport_covered.et");
			svSouth2.addItemsPrefab(itemMapM923);
			
			// US MHQ + equipment
			svSouth3.setPrefab(SCR_CTI_Constants.US_MHQ);
			svSouth3.addItemsPrefab(intemMapM923MHQ);
		} else {
			// Jeep + equipment
			svNorth1.setPrefab(SCR_CTI_Constants.US_JEEP);
			svNorth1.addItemsPrefab(itemMapM151);

			// M923A1 truck + equipment
			svNorth2.setPrefab("{084A168713BD0D21}Prefabs/Vehicles/Wheeled/M923A1/M923A1_transport_covered.et");
			svNorth2.addItemsPrefab(itemMapM923);
			
			// US MHQ + equipment
			svNorth3.setPrefab(SCR_CTI_Constants.US_MHQ);
			svNorth3.addItemsPrefab(intemMapM923MHQ);
			
			// UAZ + equipment
			svSouth1.setPrefab(SCR_CTI_Constants.USSR_UAZ);
			svSouth1.addItemsPrefab(itemMapUaz);
			
			// Ural truck + equipment
			svSouth2.setPrefab("{3B4A2C1ED5DBB0B8}Prefabs/Vehicles/Wheeled/Ural4320/Ural4320_transport_covered.et");
			svSouth2.addItemsPrefab(itemMapUral);
			
			// USSR MHQ + equipment
			svSouth3.setPrefab(SCR_CTI_Constants.USSR_MHQ);
			svSouth3.addItemsPrefab(intemMapUralMHQ);
		}
	}

	//------------------------------------------------------------------------------------------------
	void SCR_CTI_RandomStartComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
	}

	//------------------------------------------------------------------------------------------------
	void ~SCR_CTI_RandomStartComponent()
	{
	}	
};