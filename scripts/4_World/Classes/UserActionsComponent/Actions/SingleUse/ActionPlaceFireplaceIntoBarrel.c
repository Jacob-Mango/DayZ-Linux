class ActionPlaceFireplaceIntoBarrel: ActionSingleUseBase
{
	void ActionPlaceFireplaceIntoBarrel()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_OPENDOORFW;
		m_StanceMask        = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_MessageStartFail = "";
		m_MessageStart = "";
		m_MessageSuccess = "I placed the fireplace inside.";
		m_MessageFail = "I was unable to place the fireplace inside.";
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
	}
	
	override int GetType()
	{
		return AT_PLACE_FIREPLACE_BARREL;
	}
		
	override string GetText()
	{
		return "#place_fireplace";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNonRuined( UAMaxDistances.DEFAULT );
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object target_object = target.GetObject();
		
		if ( target_object && target_object.IsInherited( BarrelHoles_ColorBase ) )
		{
			BarrelHoles_ColorBase fireplace_barrel = BarrelHoles_ColorBase.Cast( target_object );
			
			if ( fireplace_barrel.IsOpened() && fireplace_barrel.GetInventory().AttachmentCount() == 0 && fireplace_barrel.IsCargoEmpty() )
			{
				return true;
			}
		}
		
		return false;
	}
	
	override void OnCompleteServer( ActionData action_data )
	{	
		FireplaceBase fireplace_in_hands = FireplaceBase.Cast( action_data.m_MainItem );
		
		BarrelHoles_ColorBase fireplace_barrel = BarrelHoles_ColorBase.Cast( action_data.m_Target.GetObject() );
		
		//transfer all attachments to this object
		int item_attachments_count = fireplace_in_hands.GetInventory().AttachmentCount();
		for ( int j = 0; j < item_attachments_count; j++ )
		{
			EntityAI entity = fireplace_in_hands.GetInventory().GetAttachmentFromIndex( 0 );
			fireplace_barrel.PredictiveTakeEntityAsAttachment( entity );
		}
		
		action_data.m_Player.GetSoftSkillManager().AddSpecialty( m_SpecialtyWeight );
	}
}
