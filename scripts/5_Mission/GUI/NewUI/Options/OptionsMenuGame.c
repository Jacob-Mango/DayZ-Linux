class OptionsMenuGame extends ScriptedWidgetEventHandler
{
	protected Widget						m_Root;
	
	protected Widget						m_SettingsRoot;
	protected Widget						m_DetailsRoot;
	protected TextWidget					m_DetailsLabel;
	protected RichTextWidget				m_DetailsText;
	
	protected ref NumericOptionsAccess		m_FOVOption;
	#ifdef PLATFORM_CONSOLE
	protected ref NumericOptionsAccess		m_BrightnessOption;
	protected ref OptionSelectorSlider		m_BrightnessSelector;
	#endif
	
	protected ref OptionSelectorSlider		m_FOVSelector;
	protected ref OptionSelectorMultistate	m_ShowHUDSelector;
	protected ref OptionSelectorMultistate	m_ShowCrosshairSelector;
	protected ref OptionSelectorMultistate	m_ShowQuickbarSelector;
	
	protected GameOptions					m_Options;
	protected OptionsMenuNew				m_Menu;
	
	protected ref map<int, ref Param2<string, string>> m_TextMap;
	
	void OptionsMenuGame( Widget parent, Widget details_root, GameOptions options, OptionsMenuNew menu )
	{
		#ifdef PLATFORM_CONSOLE
			m_Root					= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/options/xbox/game_tab.layout", parent );
		#else
		#ifdef PLATFORM_WINDOWS
			m_Root					= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/options/pc/game_tab.layout", parent );
		#endif
		#endif
		
		m_DetailsRoot				= details_root;
		m_DetailsLabel				= TextWidget.Cast( m_DetailsRoot.FindAnyWidget( "details_label" ) );
		m_DetailsText				= RichTextWidget.Cast( m_DetailsRoot.FindAnyWidget( "details_content" ) );
		
		m_Options					= options;
		m_Menu						= menu;
		
		m_FOVOption					= NumericOptionsAccess.Cast( m_Options.GetOptionByType( AT_OPTIONS_FIELD_OF_VIEW ) );
		
		m_Root.FindAnyWidget( "fov_setting_option" ).SetUserID( AT_OPTIONS_FIELD_OF_VIEW );
		m_Root.FindAnyWidget( "hud_setting_option" ).SetUserID( 1 );
		m_Root.FindAnyWidget( "crosshair_setting_option" ).SetUserID( 2 );
		
		#ifdef PLATFORM_CONSOLE
		m_Root.FindAnyWidget( "brightness_setting_option" ).SetUserID( AT_OPTIONS_BRIGHT_SLIDER );
		#else
		#ifdef PLATFORM_WINDOWS
		m_Root.FindAnyWidget( "quickbar_setting_option" ).SetUserID( 3 );
		#endif
		#endif
		
		FillTextMap();
		
		ref array<string> opt		= { "Disabled", "Enabled" };
		
		m_FOVSelector				= new OptionSelectorSlider( m_Root.FindAnyWidget( "fov_setting_option" ), m_FOVOption.ReadValue(), this, false, m_FOVOption.GetMin(), m_FOVOption.GetMax() );
		m_ShowHUDSelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "hud_setting_option" ), g_Game.GetProfileOption( EDayZProfilesOptions.HUD ), this, false, opt );
		m_ShowCrosshairSelector		= new OptionSelectorMultistate( m_Root.FindAnyWidget( "crosshair_setting_option" ), g_Game.GetProfileOption( EDayZProfilesOptions.CROSSHAIR ), this, false, opt );
		
		m_FOVSelector.m_OptionChanged.Insert( UpdateFOVOption );
		m_ShowHUDSelector.m_OptionChanged.Insert( UpdateHUDOption );
		m_ShowCrosshairSelector.m_OptionChanged.Insert( UpdateCrosshairOption );
		
		#ifdef PLATFORM_CONSOLE
			m_BrightnessOption		= NumericOptionsAccess.Cast( m_Options.GetOptionByType( AT_OPTIONS_BRIGHT_SLIDER ) );
			m_BrightnessSelector	= new OptionSelectorSlider( m_Root.FindAnyWidget( "brightness_setting_option" ), m_BrightnessOption.ReadValue(), this, false, m_BrightnessOption.GetMin(), m_BrightnessOption.GetMax() );
			m_BrightnessSelector.m_OptionChanged.Insert( UpdateBrightnessOption );
		#else
		#ifdef PLATFORM_WINDOWS
			m_ShowQuickbarSelector	= new OptionSelectorMultistate( m_Root.FindAnyWidget( "quickbar_setting_option" ), g_Game.GetProfileOption( EDayZProfilesOptions.QUICKBAR ), this, false, opt );
			m_ShowQuickbarSelector.m_OptionChanged.Insert( UpdateQuickbarOption );
		#endif
		#endif
	}
	
	void Focus()
	{
		#ifdef PLATFORM_CONSOLE
			SetFocus( m_FOVSelector.GetParent() );
		#endif
	}
	
	bool IsChanged()
	{
		#ifdef PLATFORM_CONSOLE
		return ( m_ShowHUDSelector.GetValue() != g_Game.GetProfileOption( EDayZProfilesOptions.HUD ) || m_ShowCrosshairSelector.GetValue() != g_Game.GetProfileOption( EDayZProfilesOptions.CROSSHAIR ) );
		#else
		return ( m_ShowHUDSelector.GetValue() != g_Game.GetProfileOption( EDayZProfilesOptions.HUD ) || m_ShowCrosshairSelector.GetValue() != g_Game.GetProfileOption( EDayZProfilesOptions.CROSSHAIR ) || m_ShowQuickbarSelector.GetValue() != g_Game.GetProfileOption( EDayZProfilesOptions.QUICKBAR ) );
		#endif
	}
	
	void Apply()
	{
		IngameHud hud = GetHud();
		
		g_Game.SetProfileOption( EDayZProfilesOptions.HUD, m_ShowHUDSelector.GetValue() );
		g_Game.SetProfileOption( EDayZProfilesOptions.CROSSHAIR, m_ShowCrosshairSelector.GetValue() );
		
		#ifdef PLATFORM_WINDOWS
			g_Game.SetProfileOption( EDayZProfilesOptions.QUICKBAR, m_ShowQuickbarSelector.GetValue() );
			if( hud )
			{
				hud.ToggleQuickBar( m_ShowQuickbarSelector.GetValue() );
			}
		#endif
		
		if( hud )
		{
			hud.ToggleHud( m_ShowHUDSelector.GetValue() );
		}
	}
	
	void Revert()
	{
		m_ShowHUDSelector.SetValue( g_Game.GetProfileOption( EDayZProfilesOptions.HUD ), false );
		m_ShowCrosshairSelector.SetValue( g_Game.GetProfileOption( EDayZProfilesOptions.CROSSHAIR ), false );
		m_FOVSelector.SetValue( m_FOVOption.ReadValue(), false );
		
		#ifdef PLATFORM_WINDOWS
		m_ShowQuickbarSelector.SetValue( g_Game.GetProfileOption( EDayZProfilesOptions.QUICKBAR ), false );
		#else
		#ifdef PLATFORM_CONSOLE
			m_BrightnessSelector.SetValue( m_BrightnessOption.ReadValue(), false );
		#endif
		#endif
	}
	
	void ReloadOptions()
	{
		m_Menu.ReloadOptions();
	}
	
	void SetOptions( GameOptions options )
	{
		m_Options = options;
	}
	
	void UpdateFOVOption( float new_value )
	{
		m_FOVOption.WriteValue( new_value );
		g_Game.SetUserFOV( new_value );
		m_Menu.OnChanged();
	}
	
	void UpdateHUDOption( int new_index )
	{
		m_Menu.OnChanged();
	}
	
	void UpdateCrosshairOption( int new_index )
	{
		m_Menu.OnChanged();
	}
	
	void UpdateQuickbarOption( int new_index )
	{
		m_Menu.OnChanged();
	}
	
	IngameHud GetHud()
	{
		Mission mission = GetGame().GetMission();
		if ( mission )
		{
			IngameHud hud = IngameHud.Cast( mission.GetHud() );
			return hud;
		}
		return null;
	}
	
#ifdef PLATFORM_CONSOLE
	void UpdateBrightnessOption( float value )
	{
		m_BrightnessOption.WriteValue( value );
		m_Menu.OnChanged();
	}
#endif
	
	override bool OnFocus( Widget w, int x, int y )
	{
		if( m_Menu )
			m_Menu.OnFocus( w, x, y );
		if( w )
		{
			Param2<string, string> p = m_TextMap.Get( w.GetUserID() );
			if( p )
			{
				m_DetailsRoot.Show( true );
				m_DetailsLabel.SetText( p.param1 );
				m_DetailsText.SetText( p.param2 );
				int sx, sy;
				float lines = m_DetailsText.GetContentHeight();
				m_DetailsText.SetSize( 1, lines );
				return true;
			}
		}
		m_DetailsRoot.Show( false );
		return ( w != null );
	}
	
	void FillTextMap()
	{
		m_TextMap = new map<int, ref Param2<string, string>>;
		m_TextMap.Insert( AT_OPTIONS_FIELD_OF_VIEW, new Param2<string, string>( "Field of View", "Sets the players field of view, between 60 and 110 degrees." ) );
		m_TextMap.Insert( 1, new Param2<string, string>( "Show HUD", "Toggles whether the HUD should be visible at any time. Does not affect the quickbar." ) );
		m_TextMap.Insert( 2, new Param2<string, string>( "Show Crosshair", "Toggles the visibility of the crosshair." ) );
		#ifdef PLATFORM_WINDOWS
		m_TextMap.Insert( 3, new Param2<string, string>( "Show Quickbar", "Toggles the default visibility state of the quickbar." ) );
		#else
		#ifdef PLATFORM_CONSOLE
		m_TextMap.Insert( AT_OPTIONS_BRIGHT_SLIDER, new Param2<string, string>( "Brightness", "Overall image brightness. Helps with game visibility in adverse lighting conditions." ) );
		#endif
		#endif
	}
}