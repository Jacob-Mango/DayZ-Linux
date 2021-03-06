class Environment
{
	const float WATER_LEVEL_HIGH	= 1.5;
	const float WATER_LEVEL_MID 	= 1.2;
	const float WATER_LEVEL_LOW 	= 0.5;
	const float WATER_LEVEL_NONE	= 0.15;

	protected float	   				m_DayTemp;
	protected float  				m_NightTemp;
	
	protected float					m_WetDryTick; //ticks passed since last clothing wetting or drying
	protected float					m_BuildingRCCheckTimer; // keeps info about tick time

	//player
	protected PlayerBase 			m_Player;
	protected float					m_PlayerHeightPos; // y position of player above water level (meters)
	protected float 				m_PlayerSpeed; // 1-3 speed of player movement
	protected float 				m_PlayerTemperature; //34-44
	protected float 				m_PlayerHeat; //3-9 heatcomfort generated by entites movement
	protected float					m_HeatComfort;  //delta that changes entitys temperature				
	protected float					m_ItemsHeat; // temperature of items in inventory modified by heat transfer COEFicient
	protected float					m_ItemsHeatIsolation; // 0-30  isolation of clothes, fresh spawn heatisolation = 5.5 + 2.5 + 4a cca 12, cca 20 in sweater		
	
	//environment
	protected float 				m_Rain = 0; // 0-1 amount of rain 
	protected float 				m_Wind = 0; // strength of wind
	protected float 				m_Fog = 0; // 0-1 how foggy it is
	protected float 				m_DayOrNight = 0; // 0-1 day(1) or night(0)
	protected float 				m_Clouds = 0; // 0-1 how cloudy it is
	protected float 				m_EnvironmentTemperature = 0; //temperature of environment player is in
	protected float					m_Time = 0;
	
	//
	protected float					m_WaterLevel;
	protected bool 					m_IsInside;
	protected bool 					m_IsUnderRoof;
	protected bool 					m_IsInWater;
	
	//
	protected float 				m_HeatSourceTemp;
	
	ref protected array<int> 		m_SlotIdsComplete;
	ref protected array<int> 		m_SlotIdsUpper;
	ref protected array<int> 		m_SlotIdsBottom;
	ref protected array<int> 		m_SlotIdsLower;
		
	void Environment(PlayerBase pPlayer)
	{
		Init(pPlayer);
	}
	
	void Init(PlayerBase pPlayer)
	{
		m_Player 				= pPlayer;
		m_PlayerSpeed			= 0.0;
		m_WetDryTick			= 0.0;
		m_BuildingRCCheckTimer	= 0.0;
		m_WaterLevel			= 0.0;
		m_HeatComfort			= 0.0;
		
		m_IsInside				= false;
		m_IsUnderRoof			= false;
		m_IsInWater				= false;

		//! load temperatures (from server config)
		if (GetGame().GetMission().GetWorldData())
		{
			m_DayTemp = g_Game.GetMission().GetWorldData().GetDayTemperature();
			m_NightTemp = g_Game.GetMission().GetWorldData().GetNightTemperature();
		}
		m_HeatSourceTemp		= 0.0;

		//! whole body slots		
		m_SlotIdsComplete 		= new array<int>;		
		m_SlotIdsComplete 		= {
			InventorySlots.HEADGEAR,
			InventorySlots.MASK,
			InventorySlots.EYEWEAR,
			InventorySlots.GLOVES,
			InventorySlots.ARMBAND,
			InventorySlots.BODY,
			InventorySlots.VEST,
			InventorySlots.BACK,
			InventorySlots.LEGS,
			InventorySlots.FEET,
		};
		//! upper body part slots
		m_SlotIdsUpper 			= new array<int>;
		m_SlotIdsUpper 			= {
			InventorySlots.GLOVES,
			InventorySlots.ARMBAND,
			InventorySlots.BODY,
			InventorySlots.VEST,
			InventorySlots.BACK,
			InventorySlots.LEGS,
			InventorySlots.FEET,
		};
		//! bottom body part slots
		m_SlotIdsBottom 		= new array<int>;
		m_SlotIdsBottom			= {
			InventorySlots.LEGS,
			InventorySlots.FEET,
		};
		//! lower body part slots
		m_SlotIdsLower 			= new array<int>;
		m_SlotIdsLower			= {
			InventorySlots.FEET,
		};
	}

	// Calculates heatisolation of clothing, process its wetness, collects heat from heated items and calculates player's heat comfort
	void Update(float pDelta)
	{		
		if (m_Player)
		{
			m_BuildingRCCheckTimer += pDelta;
			//! check if player is under roof (only if the Building check is false)
			if (m_BuildingRCCheckTimer >= ENVIRO_TICK_ROOF_RC_CHECK && !IsInsideBuilding())
			{
				CheckUnderRoof();
				m_BuildingRCCheckTimer = 0;
			}

			m_Time += pDelta;
			if ( m_Time >= ENVIRO_TICK_RATE )
			{
				m_Time = 0;
				m_WetDryTick++; // Sets whether it is time to add wetness to items and clothing

				//! Updates data
				CheckWaterContact(m_WaterLevel);
				CheckInsideBuilding();
				CollectAndSetPlayerData();
				CollectAndSetEnvironmentData();
				
				//! Process temperature
				ProcessItemsHeat();

				//! Process item wetness/dryness
				if ( m_WetDryTick >= ENVIRO_TICKS_TO_WETNESS_CALCULATION )
				{
					if (IsWaterContact())
					{
						ProcessWetnessByWaterLevel(m_WaterLevel);
					}
					else if (m_Rain > 0.01 && !IsInsideBuilding() && !IsUnderRoof())
					{
						ProcessWetnessByRain();
					}
					else
					{
						ProcessItemsDryness();
					}

					//! apply wet/dry to player stats too
					m_Player.GetStatWet().Add(GetWetDelta());

					m_WetDryTick = 0;
				}
			}
		}
	}
	
	void AddToEnvironmentTemperature(float pTemperature)
	{
		m_HeatSourceTemp = pTemperature;
	}
			
	//! Returns heat player generated based on player's movement speed (for now)
	protected float GetPlayerHeat()
	{
		float heat = Math.Max(m_PlayerSpeed*ENVIRO_DEFAULT_ENTITY_HEAT, ENVIRO_DEFAULT_ENTITY_HEAT);
		//float heat = Math.Max(m_PlayerSpeed*0.5, 0.5);
		return heat;
	}
	
	protected bool IsUnderRoof()
	{
		return m_IsUnderRoof;
	}
	
	protected bool IsWaterContact()
	{
		return m_IsInWater;
	}
	
	protected bool IsInsideBuilding()
	{
		return m_IsInside;
	}
	
	//! Checks whether Player is sheltered
	protected void CheckUnderRoof()
	{
		float hitFraction;
		vector hitPosition, hitNormal;
		vector from = m_Player.GetPosition();
		vector to = from + "0 25 0";
		Object hitObject;
		PhxInteractionLayers collisionLayerMask = PhxInteractionLayers.BUILDING|PhxInteractionLayers.VEHICLE;
		
		m_IsUnderRoof = DayZPhysics.RayCastBullet(from, to, collisionLayerMask, hitObject, hitPosition, hitNormal, hitFraction);
	}
	
	protected void CheckWaterContact(out float pWaterLevel)
	{
		string surfType;
		int liquidType;

		g_Game.SurfaceUnderObject(m_Player, surfType, liquidType);

		switch ( liquidType )
		{
			case 0: // sea
			case LIQUID_WATER:
				vector wl = HumanCommandSwim.WaterLevelCheck(m_Player, m_Player.GetPosition());
				pWaterLevel = wl[0];
				m_IsInWater = true;
			break;
			
			default:
				pWaterLevel = 0;
				m_IsInWater = false;
			break;
		}

		//! sync info about water contact to player
		m_Player.SetInWater(m_IsInWater);
	}
	
	protected void CheckInsideBuilding()
	{
		if (m_Player.IsSoundInsideBuilding() > 0)
		{
			m_IsInside = true;
			return;
		}

		m_IsInside = false;
	}

	// Calculates item heat isolation based on its wetness
	float GetCurrentItemHeatIsolation(ItemBase pItem)
	{
		float heat_isolation = pItem.GetHeatIsolation();
		float wet_factor = pItem.GetWet();

		//! takes into account health of item
		heat_isolation = heat_isolation * pItem.GetHealth01("", "");
		
		if ( wet_factor > 0 )
		{
			if ( wet_factor <= ENVIRO_WET_PENALTY )
			{
				heat_isolation = heat_isolation * (1 - wet_factor);
			}
			else
			{
				heat_isolation = -ENVIRO_WET_PENALTY_EFFECT * (wet_factor - ENVIRO_WET_PENALTY);
			}
		}
		return heat_isolation;
	}

	protected float GetCurrentItemWetAbsorbency(ItemBase pItem)
	{
		float absorbency = pItem.GetAbsorbency();

		//! takes into account health of item
		absorbency = absorbency + (1 - pItem.GetHealth01("", "")) * 0.25;
		return Math.Min(1, absorbency);
	}
	
	//ENVIRONTMENT
	// Returns amount of ?C air temperature should be lowered by, based on player's height above water level
	protected float GetTemperatureHeightCorrection()
	{
		float temperature_reduction = Math.Max(0, (m_PlayerHeightPos * ENVIRO_TEMPERATURE_HEIGHT_REDUCTION));
		return temperature_reduction;
	}
	
	// Calculates and return temperature of environment
	protected float GetEnvironmentTemperature()
	{
		float temperature;

		if ( m_NightTemp < m_DayTemp )
		{
			temperature = m_NightTemp + ((m_DayTemp - m_NightTemp) * m_DayOrNight);
		}
		else
		{
			temperature = m_DayTemp + ((m_NightTemp - m_DayTemp) * m_DayOrNight);
		}
		
		if ( IsWaterContact() ) 
		{
			temperature = temperature * ENVIRO_WATER_TEMPERATURE_COEF;
		}
		
		if ( IsInsideBuilding() )
		{
			temperature = temperature * ENVIRO_TEMPERATURE_INSIDE_COEF;
		}
		
		if ( IsUnderRoof() )
		{
			temperature = temperature * ENVIRO_TEMPERATURE_UNDERROOF_COEF;
		}

		float fog_effect  = m_Fog * ENVIRO_FOG_TEMP_EFFECT;
		float clouds_effect = m_Clouds * ENVIRO_CLOUDS_TEMP_EFFECT;
		//float wind_effect = m_Wind * 0.01;
		//float combined_effect = clouds_effect + fog_effect + wind_effect;
		float combined_effect = clouds_effect + fog_effect;
		if ( combined_effect > 0 )
		{
			temperature = temperature * (1 - combined_effect);
		}
		
		if (m_HeatSourceTemp > temperature)
		{
			temperature = temperature + m_HeatSourceTemp;
		}
		temperature -= GetTemperatureHeightCorrection();
		return temperature;
	}	
		
	// Calculats wet/drying delta based on player's location and weather
	protected float GetWetDelta()
	{
		float wet_delta = 0;
		if ( IsWaterContact() )
		{
			//! player is getting wet by movement/swimming in water (+differentiate wet_delta by water level)
			if (m_WaterLevel >= WATER_LEVEL_HIGH)
			{
				wet_delta = 1;
			}
			else if (m_WaterLevel >= WATER_LEVEL_MID && m_WaterLevel < WATER_LEVEL_HIGH)
			{
				wet_delta = 0.66;
			}
			else if (m_WaterLevel >= WATER_LEVEL_LOW && m_WaterLevel < WATER_LEVEL_MID)
			{
				wet_delta = 0.66;
			}
			else if (m_WaterLevel >= WATER_LEVEL_NONE && m_WaterLevel < WATER_LEVEL_LOW)
			{
				wet_delta = 0.33;
			}
		}
		else if ( m_Rain > 0.01 && !IsInsideBuilding() && !IsUnderRoof() )
		{
			//! player is getting wet from rain
			wet_delta = ENVIRO_WET_INCREMENT * ENVIRO_TICKS_TO_WETNESS_CALCULATION * (1+(ENVIRO_WIND_EFFECT*m_Wind)) * ( m_Rain );
		}
		else
		{
			//! player is drying
			float sun_effect = (ENVIRO_SUN_INCREMENT * m_DayOrNight * (1-m_Fog))*(1-(m_Clouds*ENVIRO_CLOUD_DRY_EFFECT));
			wet_delta = -((ENVIRO_DRY_INCREMENT * Math.Sqrt(m_PlayerHeat + GetEnvironmentTemperature())) + sun_effect) * (1+(ENVIRO_WIND_EFFECT*m_Wind));
		}

		return wet_delta;
	}
	

	// EXPOSURE
	// Each tick updates current entity member variables
	protected void CollectAndSetPlayerData()
	{
		vector playerPos = m_Player.GetPosition();
		m_PlayerHeightPos = playerPos[1];

		HumanCommandMove hcm = m_Player.GetCommand_Move();
		if(hcm)
			m_PlayerSpeed = hcm.GetCurrentMovementSpeed();
		m_PlayerTemperature = m_Player.GetStatTemperature().Get(); //can be current entity temeprature in future
		m_PlayerHeat = GetPlayerHeat();
	}
	
	// Each tick updates current environment member variables
	protected void CollectAndSetEnvironmentData()
	{
		Weather weather = g_Game.GetWeather();

		m_Rain = weather.GetRain().GetActual();
		m_DayOrNight = (-1 * Math.AbsFloat( ( 1 - ( g_Game.GetDayTime() / ENVIRO_HIGH_NOON ) ) ) ) + 1;
		m_Fog = weather.GetFog().GetActual();
		m_Clouds =	weather.GetOvercast().GetActual();
		m_Wind = weather.GetWindSpeed();
		m_EnvironmentTemperature = GetEnvironmentTemperature();
	}

	protected void ProcessWetnessByRain()
	{
		ProcessItemsWetness(m_SlotIdsComplete);
	}

	protected void ProcessWetnessByWaterLevel(float pWaterLevel)
	{
		ref array<int> slotIds = new array<int>;
	
		// process attachments by water depth
		if (pWaterLevel >= WATER_LEVEL_HIGH)
		{
			//! complete
			ProcessItemsWetness(m_SlotIdsComplete);
		}
		else if (pWaterLevel >= WATER_LEVEL_MID && pWaterLevel < WATER_LEVEL_HIGH)
		{
			//! upper part
			ProcessItemsWetness(m_SlotIdsUpper);
		}
		else if (pWaterLevel >= WATER_LEVEL_LOW && pWaterLevel < WATER_LEVEL_MID)
		{
			//! bottom part
			ProcessItemsWetness(m_SlotIdsBottom);
		}
		else if (pWaterLevel >= WATER_LEVEL_NONE && pWaterLevel < WATER_LEVEL_LOW)
		{
			//! feet
			ProcessItemsWetness(m_SlotIdsLower);
		}
	}

	// Wets or dry items once in given time
	protected void ProcessItemsWetness(array<int> pSlotIds)
	{
		EntityAI attachment;
		ItemBase item;
		
		int attCount = m_Player.GetInventory().AttachmentCount();
		
		for (int attIdx = 0; attIdx < attCount; attIdx++)
		{
			attachment = m_Player.GetInventory().GetAttachmentFromIndex(attIdx);
			if ( attachment.IsItemBase() )
			{
				item = Class.Cast(attachment);
				int attachmentSlot = attachment.GetInventory().GetSlotId(0);

				for (int i = 0; i < pSlotIds.Count(); i++)
				{
					if (attachmentSlot == pSlotIds.Get(i))
					{
						ApplyWetnessToItem(item);
					}
				}
			}
		}
		
		if (m_Player.GetItemInHands())
		{
			ApplyWetnessToItem(m_Player.GetItemInHands());
		}

		//! force recalc of player's load (for stamina)		
		m_Player.CalculatePlayerLoad();
	}

	protected void ProcessItemsDryness()
	{
		EntityAI attachment;
		ItemBase item;
		
		int attCount = m_Player.GetInventory().AttachmentCount();
		
		for (int attIdx = 0; attIdx < attCount; attIdx++)
		{
			attachment = m_Player.GetInventory().GetAttachmentFromIndex(attIdx);
			if ( attachment && attachment.IsItemBase() )
			{
				item = Class.Cast(attachment);
				ApplyDrynessToItem(item);
			}
		}

		if (m_Player.GetItemInHands())
		{
			ApplyDrynessToItem(m_Player.GetItemInHands());
		}

		//! force recalc of player's load (for stamina)		
		m_Player.CalculatePlayerLoad();
	}
	
	
	protected void ApplyWetnessToItem(ItemBase pItem)
	{
		if (pItem && GetCurrentItemWetAbsorbency(pItem) > 0)
		{
			pItem.AddWet(GetWetDelta() * GetCurrentItemWetAbsorbency(pItem));
			//Print("processing wetness for " + pItem.GetDisplayName());
	
			if ( pItem.GetInventory().GetCargo() )
			{
				int inItemCount = pItem.GetInventory().GetCargo().GetItemCount();
	
				for (int i = 0; i < inItemCount; i++)
				{
					ItemBase inItem;
					if ( Class.CastTo(inItem, pItem.GetInventory().GetCargo().GetItem(i)) )
					{
						inItem.AddWet(GetWetDelta() * GetCurrentItemWetAbsorbency(inItem) * ENVIRO_WET_PASSTHROUGH_COEF);
						//Print("processing wetness for (inside)" + inItem.GetDisplayName());
					}
				}
			}
		}
	}
	
	protected void ApplyDrynessToItem(ItemBase pItem)
	{
		if (pItem)
		{
			pItem.AddWet(GetWetDelta());
			//Print("drying for " + pItem.GetDisplayName());
	
			if ( pItem.GetInventory().GetCargo() )
			{
				int inItemCount = pItem.GetInventory().GetCargo().GetItemCount();
	
				for (int i = 0; i < inItemCount; i++)
				{
					ItemBase inItem;
					if ( Class.CastTo(inItem, pItem.GetInventory().GetCargo().GetItem(i)) )
					{
						inItem.AddWet(GetWetDelta() * ENVIRO_WET_PASSTHROUGH_COEF);
						//Print("drying for (inside)" + inItem.GetDisplayName());
					}
				}
			}
		}
	}

	// Calculates and process temperature of items
	protected void ProcessItemsHeat()
	{
		EntityAI attachment;
		ItemBase item;

		m_ItemsHeat = 0.0;
		m_ItemsHeatIsolation = 0.0;

		int attachment_count = m_Player.GetInventory().AttachmentCount();

		for (int att = 0; att < attachment_count; att++)
		{
			attachment = m_Player.GetInventory().GetAttachmentFromIndex(att);

			if ( attachment.IsItemBase() )
			{
				if ( Class.CastTo(item, attachment) )
				{
					if ( item.IsClothing() ) 
					{
						m_ItemsHeatIsolation += GetCurrentItemHeatIsolation(item);
					}

					if ( m_Player.GetInventory().GetAttachmentFromIndex(att).GetInventory().GetCargo() )
					{
						int icount = m_Player.GetInventory().GetAttachmentFromIndex(att).GetInventory().GetCargo().GetItemCount();

						for (int i = 0; i < icount; i++)
						{						
							ItemBase in_item;
							if ( Class.CastTo(in_item, attachment.GetInventory().GetCargo().GetItem(i)) && in_item.IsItemBase() )
							{
								m_ItemsHeat += in_item.GetTemperature();
							}
						}
					}
				}
			}
		}
		// Calculate heat gained from items based on its temeprature
		m_ItemsHeat = m_ItemsHeat * ENVIRO_ITEM_HEAT_TRANSFER_COEF;
		// Sets heatcomfort. Should be 0. More it is below 0 the colder player feels, more is it over 0 the hotter player feels.
		m_HeatComfort = ( m_PlayerHeat + m_ItemsHeat + m_ItemsHeatIsolation - ( m_PlayerTemperature - m_EnvironmentTemperature) );
		m_Player.GetStatHeatComfort().Set(m_HeatComfort);
	}

#ifdef DEVELOPER
	void ShowEnvDebugPlayerInfo(bool enabled)
	{
		int windowPosX = 10;
		int windowPosY = 200;

		Object obj;

		DbgUI.Begin("Player stats", windowPosX, windowPosY);
		if( enabled )
		{
			DbgUI.Text("Temperature: " + m_PlayerTemperature.ToString());
			DbgUI.Text("Heat comfort: " + m_HeatComfort.ToString());
			DbgUI.Text("Heat Isolation: " + m_ItemsHeatIsolation.ToString());
			DbgUI.Text("Inside: " + m_IsInside.ToString() + " ("+m_Player.GetSurfaceType()+")");
			DbgUI.Text("Under roof: " + m_IsUnderRoof.ToString());
			DbgUI.Text("Water Level: " + m_WaterLevel);
			
		}
		DbgUI.End();
		
		DbgUI.Begin("Weather stats:", windowPosX, windowPosY + 200);
		if( enabled )
		{
			DbgUI.Text("Env temperature: " +  m_EnvironmentTemperature.ToString());
			DbgUI.Text("Wind: " +  m_Wind.ToString());
			DbgUI.Text("Rain: " + m_Rain.ToString());
			DbgUI.Text("Day/Night (1/0): " + m_DayOrNight.ToString());
			DbgUI.Text("Fog: " + m_Fog.ToString());
			DbgUI.Text("Clouds: " + m_Clouds.ToString());
			DbgUI.Text("Height: " + GetTemperatureHeightCorrection().ToString());
			DbgUI.Text("Wet delta: " + GetWetDelta().ToString());
		}
		DbgUI.End();
	}
#endif
};