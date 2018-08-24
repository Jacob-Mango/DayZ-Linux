class PlayerListScriptedWidget extends ScriptedWidgetEventHandler
{
	protected Widget												m_Root;
	protected TextWidget											m_Header;
	protected Widget												m_ScrollContainer;
	protected Widget												m_Content;
	protected ref map<string, ref PlayerListEntryScriptedWidget>	m_Entries;
	
	protected ScrollBarContainer 									m_Scroller;
	protected int													m_TotalEntries;
	protected int													m_SelectedEntry;
	
	void PlayerListScriptedWidget( Widget parent, string header_text )
	{
		m_Root				= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/xbox/ingamemenu_xbox/players_info_panel.layout", parent );
		m_Header			= TextWidget.Cast( m_Root.FindAnyWidget( "header_text" ) );
		m_ScrollContainer	= m_Root.FindAnyWidget( "ScrollFrame" );
		m_Content			= m_Root.FindAnyWidget( "Content" );
		
		m_Entries			= new map<string, ref PlayerListEntryScriptedWidget>;
		
		m_Header.SetText( header_text );
		
		m_ScrollContainer.GetScript( m_Scroller );
	}
	
	void ~PlayerListScriptedWidget()
	{
		delete m_Root;
	}
	
	void FocusFirst()
	{
		if( m_Content && m_Content.GetChildren() )
			SetFocus( m_Content.GetChildren().FindAnyWidget( "Button" ) );
		m_Scroller.ScrollToTop();
	}
	
	void Reload( SyncPlayerList player_list )
	{
		if( player_list && player_list.m_PlayerList && m_Entries )
		{
			foreach( string UID, ref PlayerListEntryScriptedWidget widget : m_Entries )
			{
				SyncPlayer player_found;
				foreach( SyncPlayer player : player_list.m_PlayerList )
				{
					if( player && player.m_UID == UID )
					{
						player_found = player;
						break;
					}
				}
				if( !player_found )
				{
					RemovePlayer( UID );
				}
			}
			
			for( int i = 0; i < player_list.m_PlayerList.Count(); i++ )
			{
				SyncPlayer player2 = player_list.m_PlayerList.Get( i );
				PlayerListEntryScriptedWidget player_widget;
				m_Entries.Find( player2.m_UID, player_widget );
				if( !player_widget )
				{
					AddPlayer( player2.m_PlayerName, player2.m_UID, true );
				}
			}
		}
	}
	
	bool IsEmpty()
	{
		return ( m_Entries.Count() == 0 );
	}
	
	void Reload( BiosFriendInfoArray player_list )
	{
		if( player_list && m_Entries )
		{
			foreach( string UID, ref PlayerListEntryScriptedWidget widget : m_Entries )
			{
				BiosFriendInfo player_found;
				foreach( BiosFriendInfo player : player_list )
				{
					if( player && player.m_Uid == UID )
					{
						player_found = player;
						break;
					}
				}
				if( !player_found )
				{
					RemovePlayer( UID );
				}
			}
			
			for( int i = 0; i < player_list.Count(); i++ )
			{
				BiosFriendInfo player2 = player_list.Get( i );
				PlayerListEntryScriptedWidget player_widget;
				m_Entries.Find( player2.m_Uid, player_widget );
				if( !player_widget )
				{
					AddPlayer( player2.m_DisplayName, player2.m_Uid, false );
				}
			}
		}
	}
	
	void Reload( BiosPrivacyUidResultArray player_list )
	{
		foreach( BiosPrivacyUidResult result : player_list )
		{
			PlayerListEntryScriptedWidget player_widget;
			m_Entries.Find( result.m_Uid, player_widget );
			if( player_widget )
			{
				player_widget.LoadPermissions( result.m_Results );
			}
		}
	}
	
	void ReloadLocal( map<string, bool> player_list )
	{
		if( player_list )
		{
			for( int i = 0; i < player_list.Count(); i++ )
			{
				string uid = player_list.GetKey( i );
				bool muted = player_list.Get( uid );
				PlayerListEntryScriptedWidget player_widget;
				m_Entries.Find( uid, player_widget );
				if( player_widget )
				{
					player_widget.MutePlayer( muted );
				}
			}
		}
	}
	
	string FindPlayerByWidget( Widget button )
	{
		if( button && m_Entries )
		{
			foreach( string UID, ref PlayerListEntryScriptedWidget widget : m_Entries )
			{
				if( widget && widget.GetButtonWidget() == button )
				{
					return UID;
				}
			}
		}
		
		return "";
	}
	
	void AddPlayer( string name, string UID, bool show_permissions )
	{
		if( m_Entries )
		{
			m_Entries.Insert( UID, new PlayerListEntryScriptedWidget( m_Content, name, UID, show_permissions ) );
			m_TotalEntries++;
		}
	}
	
	void RemovePlayer( string UID )
	{
		if( m_Entries )
		{
			m_Entries.Remove( UID );
			m_TotalEntries--;
			m_Content.Update();
		}
	}
	
	bool IsGloballyMuted( string UID )
	{
		if( m_Entries && m_Entries.Get( UID ) )
		{
			return m_Entries.Get( UID ).IsGloballyMuted();
		}
		return false;
	}
	
	void MutePlayer( string UID, bool mute )
	{
		if( m_Entries )
		{
			m_Entries.Get( UID ).MutePlayer( mute );
		}
	}
	
	void SetNextActive()
	{
		if( m_Content && m_Content.GetChildren() )
		{
			float x_p, y_p;
			float x_s, y_s;
			
			Widget sibling = GetFocus().GetParent().GetSibling();
			if( sibling )
			{
				sibling.GetPos( x_p, y_p );
				sibling.GetSize( x_s, y_s );
				float pos = y_p + y_s;
				float contentypos = -m_Scroller.GetContentYPos();
				float root_height = m_Scroller.GetRootHeight();
				if( pos >= root_height + contentypos )
				{
					m_Scroller.ScrollToPos( pos - m_Scroller.GetRootHeight() );
				}
			}
			else
			{
				m_Scroller.ScrollToTop();
			}
		}
	}
	
	void SetPreviousActive()
	{
		if( m_Content && m_Content.GetChildren() )
		{
			float x, y;
			float x_s, y_s;
			
			GetFocus().GetParent().GetPos( x, y );
			GetFocus().GetParent().GetSize( x_s, y_s );
			
			float amount = y - y_s;
			float limit = m_Scroller.GetContentYPos();
			
			if( m_Content.GetChildren() == GetFocus().GetParent() )
			{
				m_Scroller.ScrollToBottom();
				SetFocus( m_Content.GetChildren() );
			}
			else if( amount < -limit )
			{
				m_Scroller.ScrollToPos( amount );
			}
		}
	}
}