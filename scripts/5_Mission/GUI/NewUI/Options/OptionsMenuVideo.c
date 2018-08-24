class OptionsMenuVideo extends ScriptedWidgetEventHandler
{
	protected Widget						m_Root;
	
	protected Widget						m_SettingsRoot;
	protected Widget						m_DetailsRoot;
	protected TextWidget					m_DetailsLabel;
	protected RichTextWidget				m_DetailsText;
	
	protected GameOptions					m_Options;
	protected OptionsMenuNew				m_Menu;
	
	//Overall
	protected ref OptionSelectorMultistate	m_OverallQualitySelector;
	
	//Screen
	protected ref OptionSelectorMultistate	m_DisplayModeSelector;
	protected ref OptionSelectorMultistate	m_ResolutionSelector;
	protected ref OptionSelectorSlider		m_BrightnessSelector;
	protected ref OptionSelectorMultistate	m_VSyncSelector;
	protected ref OptionSelectorMultistate	m_ColorDepthSelector;
	
	//Scene
	protected ref OptionSelectorMultistate	m_ObjectDetailSelector;
	protected ref OptionSelectorMultistate	m_TerrainDetailSelector;
	protected ref OptionSelectorMultistate	m_TextureDetailSelector;
	protected ref OptionSelectorMultistate	m_CloudsDetailSelector;
	protected ref OptionSelectorMultistate	m_ShadowDetailSelector;
	
	//Rendering
	protected ref OptionSelectorMultistate	m_TextureFilteringSelector;
	protected ref OptionSelectorMultistate	m_TerrainSurfaceDetailSelector;
	protected ref OptionSelectorMultistate	m_PPAASelector;
	protected ref OptionSelectorMultistate	m_HWAASelector;
	protected ref OptionSelectorMultistate	m_ATOCSelector;
	protected ref OptionSelectorMultistate	m_AOSelector;
	protected ref OptionSelectorMultistate	m_PPQualitySelector;
	
	//Overall
	protected ref ListOptionsAccess			m_OverallQualityOption;
	
	//Screen
	protected ref ListOptionsAccess			m_DisplayModeOption;
	protected ref ListOptionsAccess			m_ResolutionOption;
	protected ref NumericOptionsAccess		m_BrightnessOption;
	protected ref ListOptionsAccess			m_VSyncOption;
	protected ref ListOptionsAccess			m_ColorDepthOption;
	
	//Scene
	protected ref ListOptionsAccess			m_ObjectDetailOption;
	protected ref ListOptionsAccess			m_TerrainDetailOption;
	protected ref ListOptionsAccess			m_TextureDetailOption;
	protected ref ListOptionsAccess			m_CloudsDetailOption;
	protected ref ListOptionsAccess			m_ShadowDetailOption;
	
	//Rendering
	protected ref ListOptionsAccess			m_TextureFilteringOption;
	protected ref ListOptionsAccess			m_TerrainSurfaceDetailOption;
	protected ref ListOptionsAccess			m_PPAAOption;
	protected ref ListOptionsAccess			m_HWAAOption;
	protected ref ListOptionsAccess			m_ATOCOption;
	protected ref ListOptionsAccess			m_AOOption;
	protected ref ListOptionsAccess			m_PPQualityOption;
	
	protected ref map<int, ref Param2<string, string>> m_TextMap;
	
	void OptionsMenuVideo( Widget parent, Widget details_root, GameOptions options, OptionsMenuNew menu )
	{
		m_Root							= GetGame().GetWorkspace().CreateWidgets( "gui/layouts/new_ui/options/pc/video_tab.layout", parent );
		m_DetailsRoot					= details_root;
		m_DetailsLabel					= TextWidget.Cast( m_DetailsRoot.FindAnyWidget( "details_label" ) );
		m_DetailsText					= RichTextWidget.Cast( m_DetailsRoot.FindAnyWidget( "details_content" ) );
		
		SetOptions( options );
		m_Menu							= menu;
		
		m_Root.FindAnyWidget( "overall_quality_setting_option" ).SetUserID( AT_QUALITY_PREFERENCE );
		m_Root.FindAnyWidget( "display_mode_setting_option" ).SetUserID( AT_QUALITY_PREFERENCE );
		m_Root.FindAnyWidget( "resolution_setting_option" ).SetUserID( AT_OPTIONS_RESOLUTION );
		m_Root.FindAnyWidget( "brightness_setting_option" ).SetUserID( AT_OPTIONS_BRIGHT_SLIDER );
		m_Root.FindAnyWidget( "vsync_setting_option" ).SetUserID( AT_VSYNC_VALUE );
		m_Root.FindAnyWidget( "color_depth_setting_option" ).SetUserID( AT_HDR_DETAIL );
		
		//Scene
		m_Root.FindAnyWidget( "object_detail_setting_option" ).SetUserID( AT_OBJECTS_DETAIL );
		m_Root.FindAnyWidget( "terrain_detail_setting_option" ).SetUserID( AT_OPTIONS_TERRAIN );
		m_Root.FindAnyWidget( "texture_detail_setting_option" ).SetUserID( AT_TEXTURE_DETAIL );
		m_Root.FindAnyWidget( "clouds_detail_option" ).SetUserID( AT_OPTIONS_SW_VALUE );
		m_Root.FindAnyWidget( "shadow_detail_setting_option" ).SetUserID( AT_SHADOW_DETAIL );
		
		//Rendering
		m_Root.FindAnyWidget( "texture_filtering_setting_option" ).SetUserID( AT_ANISO_DETAIL );
		m_Root.FindAnyWidget( "terrain_surface_detail_setting_option" ).SetUserID( AT_ANISO_DETAIL );
		m_Root.FindAnyWidget( "ppaa_setting_option" ).SetUserID( AT_OPTIONS_FXAA_VALUE );
		m_Root.FindAnyWidget( "hwaa_setting_option" ).SetUserID( AT_FSAA_DETAIL );
		m_Root.FindAnyWidget( "atoc_setting_option" ).SetUserID( AT_ATOC_DETAIL );
		m_Root.FindAnyWidget( "ao_setting_option" ).SetUserID( AT_AMBIENT_OCCLUSION );
		m_Root.FindAnyWidget( "pp_setting_option" ).SetUserID( AT_POSTPROCESS_EFFECTS );
		
		FillTextMap();
		
		ref array<string> opt1			= { "Disabled", "Enabled" };
		ref array<string> opt2			= { "Fullscreen", "Windowed" };
		ref array<string> opt3			= { "Low", "Medium", "High" };
		ref array<string> opt4			= { "Poor", "Low", "Medium", "High", "Extreme" };
		ref array<string> opt5			= { "Poor", "Low", "Medium", "High", "Extreme", "Custom" };
		ref array<string> opt6			= { "Disabled", "Low", "Medium", "High", "Extreme" };
		ref array<string> opt7			= { "Disabled", "Low", "Medium", "High" };
		ref array<string> opt8			= new array<string>;
		
		for( int i = 0; i < m_ResolutionOption.GetItemsCount(); i++ )
		{
			string opt_text;
			m_ResolutionOption.GetItemText( i, opt_text );
			opt8.Insert( opt_text );
		}
		
		//Overall
		m_OverallQualitySelector		= new OptionSelectorMultistate( m_Root.FindAnyWidget( "overall_quality_setting_option" ), m_OverallQualityOption.GetIndex(), this, false, opt5 );
		
		//Screen
		m_DisplayModeSelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "display_mode_setting_option" ), 0, this, false, opt2 );
		m_ResolutionSelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "resolution_setting_option" ), m_ResolutionOption.GetIndex(), this, false, opt8 );
		m_BrightnessSelector			= new OptionSelectorSlider( m_Root.FindAnyWidget( "brightness_setting_option" ), m_BrightnessOption.ReadValue(), this, false, m_BrightnessOption.GetMin(), m_BrightnessOption.GetMax() );
		m_VSyncSelector					= new OptionSelectorMultistate( m_Root.FindAnyWidget( "vsync_setting_option" ), m_VSyncOption.GetIndex(), this, false, opt1 );
		m_ColorDepthSelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "color_depth_setting_option" ), m_ColorDepthOption.GetIndex(), this, false, opt3 );
		
		//Scene
		m_ObjectDetailSelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "object_detail_setting_option" ), m_ObjectDetailOption.GetIndex(), this, false, opt4 );
		m_TerrainDetailSelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "terrain_detail_setting_option" ), m_TerrainDetailOption.GetIndex(), this, false, opt4 );
		m_TextureDetailSelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "texture_detail_setting_option" ), m_TextureDetailOption.GetIndex(), this, false, opt4 );
		m_CloudsDetailSelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "clouds_detail_option" ), m_CloudsDetailOption.GetIndex(), this, false, opt4 );
		m_ShadowDetailSelector			= new OptionSelectorMultistate( m_Root.FindAnyWidget( "shadow_detail_setting_option" ), m_ShadowDetailOption.GetIndex(), this, false, opt4 );
		
		//Rendering
		m_TextureFilteringSelector		= new OptionSelectorMultistate( m_Root.FindAnyWidget( "texture_filtering_setting_option" ), m_TextureFilteringOption.GetIndex(), this, false, opt3 );
		m_TerrainSurfaceDetailSelector	= new OptionSelectorMultistate( m_Root.FindAnyWidget( "terrain_surface_detail_setting_option" ), 0, this, false, opt4 );
		m_PPAASelector					= new OptionSelectorMultistate( m_Root.FindAnyWidget( "ppaa_setting_option" ), m_PPAAOption.GetIndex(), this, false, opt6 );
		m_HWAASelector					= new OptionSelectorMultistate( m_Root.FindAnyWidget( "hwaa_setting_option" ), m_HWAAOption.GetIndex(), this, false, opt7 );
		m_ATOCSelector					= new OptionSelectorMultistate( m_Root.FindAnyWidget( "atoc_setting_option" ), m_ATOCOption.GetIndex(), this, false, opt1 );
		m_AOSelector					= new OptionSelectorMultistate( m_Root.FindAnyWidget( "ao_setting_option" ), m_AOOption.GetIndex(), this, false, opt6 );
		m_PPQualitySelector				= new OptionSelectorMultistate( m_Root.FindAnyWidget( "pp_setting_option" ), m_PPQualityOption.GetIndex(), this, false, opt3 );
		
		//Overall
		m_OverallQualitySelector.m_OptionChanged.Insert( OnOverallQualityChanged );
		
		//Screen
		m_DisplayModeSelector.m_OptionChanged.Insert( OnDisplayModeChanged );
		m_ResolutionSelector.m_OptionChanged.Insert( OnResolutionChanged );
		m_BrightnessSelector.m_OptionChanged.Insert( OnBrightnessChanged );
		m_VSyncSelector.m_OptionChanged.Insert( OnVSyncChanged );
		m_ColorDepthSelector.m_OptionChanged.Insert( OnColorDepthChanged );
		
		//Scene
		m_ObjectDetailSelector.m_OptionChanged.Insert( OnObjectDetailChanged );
		m_TerrainDetailSelector.m_OptionChanged.Insert( OnTerrainDetailChanged );
		m_TextureDetailSelector.m_OptionChanged.Insert( OnTextureDetailChanged );
		m_CloudsDetailSelector.m_OptionChanged.Insert( OnCloudsDetailChanged );
		m_ShadowDetailSelector.m_OptionChanged.Insert( OnShadowDetailChanged );
		
		//Rendering
		m_TextureFilteringSelector.m_OptionChanged.Insert( OnTextureFilteringChanged );
		m_TerrainSurfaceDetailSelector.m_OptionChanged.Insert( OnTerrainSurfaceDetailChanged );
		m_PPAASelector.m_OptionChanged.Insert( OnPPAAChanged );
		m_HWAASelector.m_OptionChanged.Insert( OnHWAAChanged );
		m_ATOCSelector.m_OptionChanged.Insert( OnATOCChanged );
		m_AOSelector.m_OptionChanged.Insert( OnAOChanged );
		m_PPQualitySelector.m_OptionChanged.Insert( OnPPQualityChanged );
		
		m_DisplayModeSelector.Disable();
		m_TerrainSurfaceDetailSelector.Disable();

		RefreshCustom();
		GetGame().BeginOptionsVideo();
	}
	
	void ~OptionsMenuVideo()
	{
		GetGame().EndOptionsVideo();
	}
	
	void Focus()
	{
		#ifdef PLATFORM_CONSOLE
			SetFocus( m_OverallQualitySelector.GetParent() );
		#endif
	}
	
	void ReloadOptions()
	{
		m_Menu.ReloadOptions();
	}
	
	bool IsChanged()
	{
		return false;
	}
	
	void Apply()
	{
		
	}
	
	void Revert()
	{
		//m_DisplayModeSelector.SetValue( m_DisplayModeOption.GetIndex(), false );
		m_ResolutionSelector.SetValue( m_ResolutionOption.GetIndex(), false );
		m_BrightnessSelector.SetValue( m_BrightnessOption.ReadValue(), false );
		m_VSyncSelector.SetValue( m_VSyncOption.GetIndex(), false );
		m_ColorDepthSelector.SetValue( m_ColorDepthOption.GetIndex(), false );
		
		RefreshCustom();
	}
	
	void SetOptions( GameOptions options )
	{
		m_Options						= options;
		
		//Overall
		m_OverallQualityOption			= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_QUALITY_PREFERENCE ) );
		
		//Screen
		//m_DisplayModeOption			= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_OPTIONS_FIELD_OF_VIEW ) );
		m_ResolutionOption				= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_OPTIONS_RESOLUTION ) );
		m_BrightnessOption				= NumericOptionsAccess.Cast( m_Options.GetOptionByType( AT_OPTIONS_BRIGHT_SLIDER ) );
		m_VSyncOption					= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_VSYNC_VALUE ) );
		m_ColorDepthOption				= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_HDR_DETAIL ) );
		
		//Scene
		m_ObjectDetailOption			= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_OBJECTS_DETAIL ) );
		m_TerrainDetailOption			= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_OPTIONS_TERRAIN ) );
		m_TextureDetailOption			= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_TEXTURE_DETAIL ) );
		m_CloudsDetailOption			= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_OPTIONS_SW_VALUE ) );
		m_ShadowDetailOption			= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_SHADOW_DETAIL ) );
		
		//Rendering
		m_TextureFilteringOption		= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_ANISO_DETAIL ) );
		//m_TerrainSurfaceDetailOption	= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_OPTIONS_FXAA_VALUE ) );
		m_PPAAOption					= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_OPTIONS_FXAA_VALUE ) );
		m_HWAAOption					= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_FSAA_DETAIL ) );
		m_ATOCOption					= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_ATOC_DETAIL ) );
		m_AOOption						= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_AMBIENT_OCCLUSION ) );
		m_PPQualityOption				= ListOptionsAccess.Cast( m_Options.GetOptionByType( AT_POSTPROCESS_EFFECTS ) );
	}
	
	void RefreshCustom()
	{
		//Scene
		m_ObjectDetailSelector.SetValue( m_ObjectDetailOption.GetIndex(), false );
		m_TerrainDetailSelector.SetValue( m_TerrainDetailOption.GetIndex(), false );
		m_TextureDetailSelector.SetValue( m_TextureDetailOption.GetIndex(), false );
		m_CloudsDetailSelector.SetValue( m_CloudsDetailOption.GetIndex(), false );
		m_ShadowDetailSelector.SetValue( m_ShadowDetailOption.GetIndex(), false );
		
		//Rendering
		m_TextureFilteringSelector.SetValue( m_TextureFilteringOption.GetIndex(), false );
		m_TerrainSurfaceDetailSelector.SetValue( 0, false );
		m_PPAASelector.SetValue( m_PPAAOption.GetIndex(), false );
		m_HWAASelector.SetValue( m_HWAAOption.GetIndex(), false );
		if( m_HWAAOption.GetIndex() == 0 )
		{
			m_ATOCSelector.Disable();
		}
		else
		{
			m_ATOCSelector.Enable();
		}
		m_ATOCSelector.SetValue( m_ATOCOption.GetIndex(), false );
		m_AOSelector.SetValue( m_AOOption.GetIndex(), false );
		m_PPQualitySelector.SetValue( m_PPQualityOption.GetIndex(), false );
	}
	
	void OnOptionChanged()
	{
		if( m_OverallQualityOption.GetIndex() != 5 )
		{
			m_OverallQualitySelector.SetValue( 5 );
		}
	}
	
	void OnOverallQualityChanged( int value )
	{
		m_Options.Apply();
		m_OverallQualityOption.SetIndex( value );
		m_Options.Apply();
		
		ReloadOptions();
		m_OverallQualityOption.SetIndex( value );
		RefreshCustom();
	}
	
	void OnDisplayModeChanged( int value )
	{
		m_DisplayModeOption.SetIndex( value );
		m_Menu.OnChanged();
	}
	
	void OnResolutionChanged( int value )
	{
		m_ResolutionOption.SetIndex( value );
		m_Menu.OnChanged();
	}
	
	void OnBrightnessChanged( float value )
	{
		m_BrightnessOption.WriteValue( value );
		m_Menu.OnChanged();
	}
	
	void OnVSyncChanged( float value )
	{
		m_VSyncOption.SetIndex( value );
		m_Menu.OnChanged();
	}
	
	void OnColorDepthChanged( int value )
	{
		m_ColorDepthOption.SetIndex( value );
		m_Menu.OnChanged();
	}
	
	void OnObjectDetailChanged( int value )
	{
		m_ObjectDetailOption.SetIndex( value );
		OnOptionChanged();
		m_Menu.OnChanged();
	}
	
	void OnTerrainDetailChanged( int value )
	{
		m_TerrainDetailOption.SetIndex( value );
		OnOptionChanged();
		m_Menu.OnChanged();
	}
	
	void OnTextureDetailChanged( int value )
	{
		m_TextureDetailOption.SetIndex( value );
		OnOptionChanged();
		m_Menu.OnChanged();
	}
	
	void OnCloudsDetailChanged( int value )
	{
		m_CloudsDetailOption.SetIndex( value );
		OnOptionChanged();
		m_Menu.OnChanged();
	}
	
	void OnShadowDetailChanged( int value )
	{
		m_ShadowDetailOption.SetIndex( value );
		OnOptionChanged();
		m_Menu.OnChanged();
	}
	
	void OnTextureFilteringChanged( int value )
	{
		m_TextureFilteringOption.SetIndex( value );
		OnOptionChanged();
		m_Menu.OnChanged();
	}
	
	void OnTerrainSurfaceDetailChanged( int value )
	{
		//m_TerrainSurfaceDetailOption.SetIndex( value );
		OnOptionChanged();
		m_Menu.OnChanged();
	}
	
	void OnPPAAChanged( int value )
	{
		m_PPAAOption.SetIndex( value );
		OnOptionChanged();
		m_Menu.OnChanged();
	}
	
	void OnHWAAChanged( int value )
	{
		m_HWAAOption.SetIndex( value );
		if( value == 0 )
		{
			m_ATOCSelector.Disable();
		}
		else
		{
			m_ATOCSelector.Enable();
		}
		OnOptionChanged();
		m_Menu.OnChanged();
	}
	
	void OnATOCChanged( int value )
	{
		m_ATOCOption.SetIndex( value );
		OnOptionChanged();
		m_Menu.OnChanged();
	}
	
	void OnAOChanged( int value )
	{
		m_AOOption.SetIndex( value );
		OnOptionChanged();
		m_Menu.OnChanged();
	}
	
	void OnPPQualityChanged( int value )
	{
		m_PPQualityOption.SetIndex( value );
		OnOptionChanged();
		m_Menu.OnChanged();
	}
	
	override bool OnFocus( Widget w, int x, int y )
	{
		OptionsMenuNew menu = OptionsMenuNew.Cast( GetGame().GetUIManager().GetMenu() );
		if( menu )
			menu.OnFocus( w, x, y );
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
		m_TextMap.Insert( AT_QUALITY_PREFERENCE, new Param2<string, string>( "Quality", "Select predefined video quality preset, or set a custom one.\nSelect overall video quality from predefined presets of following settings, or set a custom one." ) );
		//m_TextMap.Insert( , new Param2<string, string>( "Display Mode", "Fullscreen mode makes the game to cover whole display in exclusive access. In windowed mode the game behaves like typical window application with frame around. When windowed mode is set and the resolution is selected to native display resolution, the game is in fullscreen borderless mode covering full screen but without exclusive access." ) );
		m_TextMap.Insert( AT_OPTIONS_RESOLUTION, new Param2<string, string>( "Resolution", "Amount of pixels on screen. Higher means better image quality at the expense of frame rate. Available resolutions are derived from monitor native resolution and its aspect ratio. Using highest resolution available in windowed display mode result in fullscreen borderless window." ) );
		m_TextMap.Insert( AT_OPTIONS_BRIGHT_SLIDER, new Param2<string, string>( "Brightness", "Overall image brightness. To see the game world better when it's dark while your screen is being shined on by outside source of light." ) );
		m_TextMap.Insert( AT_VSYNC_VALUE, new Param2<string, string>( "V-Sync", "Removes the unwanted vertical tearing effect at the expense of frame rate." ) );
		m_TextMap.Insert( AT_HDR_DETAIL, new Param2<string, string>( "Color Depth", "Image computation mode, in 8 bits, 10 bits or 16 bits, higher means better quality but with possible higher performance price.\nLow - 8-bit\nMedium - 10-bit\nHigh - 16-bit" ) );
		m_TextMap.Insert( AT_OBJECTS_DETAIL, new Param2<string, string>( "Object Detail", "Sets the level of detail threshold on all 3D models." ) );
		m_TextMap.Insert( AT_OPTIONS_TERRAIN, new Param2<string, string>( "Terrain Detail", "Resolution of textures." ) );
		m_TextMap.Insert( AT_TEXTURE_DETAIL, new Param2<string, string>( "Texture Detail", "Quality of clouds." ) );
		m_TextMap.Insert( AT_OPTIONS_SW_VALUE, new Param2<string, string>( "Clouds Detail", "Quality of shadows." ) );
		m_TextMap.Insert( AT_SHADOW_DETAIL, new Param2<string, string>( "Shadow Detail", "Quality of shadows." ) );
		m_TextMap.Insert( AT_ANISO_DETAIL, new Param2<string, string>( "Textures Filtering", "Increases texture quality under sharp angles while also reducing unwanted noise patterns.\nLow - Trilinear\nMedium - 4x Anisotropic\nHigh - 16x Anisotropic" ) );
		//m_TextMap.Insert( , new Param2<string, string>( "Terrain Surface Detail", "" ) );
		m_TextMap.Insert( AT_OPTIONS_FXAA_VALUE, new Param2<string, string>( "Anti-aliasing Post Process", "Increases sharpness of all edges while also reducing noise and unwanted rasterization effect. Expect heavy GPU performance hit with this on high settings!\nLow - FXAA Medium\nMedium - FXAA High\nHigh - SMAA Medium\nExtreme - SMAA High" ) );
		m_TextMap.Insert( AT_FSAA_DETAIL, new Param2<string, string>( "Anti-aliasing Hardware", "Edge smoothing, supported by video hardware. Very performance and memory intensive!\nLow - 2x MSAA\nMedium - 4x MSAA\nHigh - 8x MSAA" ) );
		m_TextMap.Insert( AT_ATOC_DETAIL, new Param2<string, string>( "Foliage Smoothing", "Edge smoothing of foliage by ATOC (alpha to overlay coverage), HW antialiasing must be enabled to support this technique." ) );
		m_TextMap.Insert( AT_AMBIENT_OCCLUSION, new Param2<string, string>( "Ambient Occlusion", "Shading and rendering technique used to calculate how exposed each point in a scene is to ambient lighting. Helps objects to stand out better from their background.\nLow - SSAO Medium\nMedium - SSAO High\nHigh - HBAO Medium\nExtreme - HBAO High" ) );
		m_TextMap.Insert( AT_POSTPROCESS_EFFECTS, new Param2<string, string>( "Post Process Quality", "Sets the quality of Postprocess effects." ) );
	}
}