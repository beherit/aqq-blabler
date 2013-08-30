object BlablerForm: TBlablerForm
  Left = 0
  Top = 3
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Blabler - ustawienia'
  ClientHeight = 330
  ClientWidth = 335
  Color = clBtnFace
  Font.Charset = EASTEUROPE_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel: TsBevel
    Left = 0
    Top = 292
    Width = 335
    Height = 38
    Align = alBottom
    Shape = bsTopLine
    ExplicitLeft = 224
    ExplicitTop = 280
    ExplicitWidth = 50
  end
  object SaveButton: TsButton
    Left = 252
    Top = 300
    Width = 75
    Height = 25
    Caption = 'Zastosuj'
    Enabled = False
    TabOrder = 0
    OnClick = SaveButtonClick
    SkinData.SkinSection = 'BUTTON'
  end
  object CancelButton: TsButton
    Left = 171
    Top = 300
    Width = 75
    Height = 25
    Caption = 'Anuluj'
    TabOrder = 1
    OnClick = aExitExecute
    SkinData.SkinSection = 'BUTTON'
  end
  object OKButton: TsButton
    Left = 90
    Top = 300
    Width = 75
    Height = 25
    Caption = 'OK'
    TabOrder = 2
    OnClick = OKButtonClick
    SkinData.SkinSection = 'BUTTON'
  end
  object sPageControl: TsPageControl
    AlignWithMargins = True
    Left = 6
    Top = 6
    Width = 323
    Height = 280
    Margins.Left = 6
    Margins.Top = 6
    Margins.Right = 6
    Margins.Bottom = 6
    ActivePage = AvatarsTabSheet
    Align = alClient
    MultiLine = True
    TabOrder = 3
    SkinData.SkinSection = 'PAGECONTROL'
    object AvatarsTabSheet: TsTabSheet
      Caption = 'Awatary'
      SkinData.CustomColor = False
      SkinData.CustomFont = False
      object AvatarsSizeGroupBox: TsGroupBox
        Left = 7
        Top = 3
        Width = 299
        Height = 71
        Caption = 'Rozmiar awatar'#243'w'
        TabOrder = 0
        SkinData.SkinSection = 'GROUPBOX'
        object PixelInfoLabel: TsLabel
          Left = 119
          Top = 23
          Width = 153
          Height = 13
          Caption = '(warto'#347#263' wyra'#380'ona w pikselach)'
          Enabled = False
        end
        object AvatarInfoLabel: TsLabel
          Left = 14
          Top = 48
          Width = 273
          Height = 13
          SkinSection = 'LABEL'
          Caption = 'Rozmiar awatar'#243'w mo'#380'e by'#263' ustalony przez kompozycj'#281'!'
          Enabled = False
        end
        object XSizeLabel: TsLabel
          Left = 75
          Top = 23
          Width = 6
          Height = 13
          Caption = 'x'
        end
        object AvatarHeightEdit: TsEdit
          Left = 87
          Top = 20
          Width = 26
          Height = 21
          TabStop = False
          Alignment = taCenter
          Font.Charset = EASTEUROPE_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          MaxLength = 3
          ParentFont = False
          ReadOnly = True
          TabOrder = 1
          Text = '25'
          SkinData.SkinSection = 'EDIT'
          BoundLabel.Indent = 0
          BoundLabel.Font.Charset = DEFAULT_CHARSET
          BoundLabel.Font.Color = clWindowText
          BoundLabel.Font.Height = -11
          BoundLabel.Font.Name = 'Tahoma'
          BoundLabel.Font.Style = []
          BoundLabel.Layout = sclLeft
          BoundLabel.MaxWidth = 0
          BoundLabel.UseSkinColor = True
        end
        object AvatarWidthCSpinEdit: TsSpinEdit
          Left = 28
          Top = 20
          Width = 41
          Height = 21
          Alignment = taCenter
          Font.Charset = EASTEUROPE_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          Text = '25'
          OnChange = AvatarWidthCSpinEditChange
          SkinData.SkinSection = 'EDIT'
          BoundLabel.Indent = 0
          BoundLabel.Font.Charset = DEFAULT_CHARSET
          BoundLabel.Font.Color = clWindowText
          BoundLabel.Font.Height = -11
          BoundLabel.Font.Name = 'Tahoma'
          BoundLabel.Font.Style = []
          BoundLabel.Layout = sclLeft
          BoundLabel.MaxWidth = 0
          BoundLabel.UseSkinColor = True
          MaxValue = 120
          MinValue = 15
          Value = 25
        end
      end
      object AvatarsUpdateGroupBox: TsGroupBox
        Left = 8
        Top = 128
        Width = 299
        Height = 114
        Caption = 'Aktualizacja awatar'#243'w'
        TabOrder = 1
        SkinData.SkinSection = 'GROUPBOX'
        object LastAvatarsUpdateLabel: TsLabelFX
          Left = 144
          Top = 47
          Width = 60
          Height = 14
          Caption = 'brak danych'
          Color = clBtnFace
          ParentColor = False
          ParentFont = False
          Font.Charset = EASTEUROPE_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          Angle = 0
          Kind.KindType = ktCustom
          Kind.Color = clGreen
          Shadow.BlurCount = 0
          Shadow.OffsetKeeper.LeftTop = 0
          Shadow.OffsetKeeper.RightBottom = 1
        end
        object LastAvatarsUpdateInfoLabel: TsLabel
          Left = 38
          Top = 47
          Width = 100
          Height = 13
          Caption = 'Ostania aktualizacja:'
          ParentFont = False
          Font.Charset = EASTEUROPE_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
        end
        object ProgressLabel: TsLabel
          Left = 18
          Top = 90
          Width = 100
          Height = 13
          Caption = 'Pobieranie danych...'
          Visible = False
        end
        object FileListBox: TFileListBox
          Left = 175
          Top = 74
          Width = 115
          Height = 25
          FileType = [ftReadOnly, ftHidden, ftSystem, ftArchive, ftNormal]
          ItemHeight = 13
          TabOrder = 2
          Visible = False
        end
        object ManualAvatarsUpdateButton: TsButton
          Left = 175
          Top = 74
          Width = 115
          Height = 25
          Caption = 'Sprawd'#378' aktualizacje'
          TabOrder = 1
          OnClick = ManualAvatarsUpdateButtonClick
          SkinData.SkinSection = 'BUTTON'
        end
        object ProgressBar: TsProgressBar
          Left = 14
          Top = 76
          Width = 147
          Height = 15
          TabOrder = 3
          Visible = False
          SkinData.SkinSection = 'GAUGE'
        end
        object AutoAvatarsUpdateComboBox: TsComboBox
          Left = 205
          Top = 20
          Width = 80
          Height = 21
          Alignment = taLeftJustify
          BoundLabel.Active = True
          BoundLabel.Caption = 'Automatyczna aktualizacje awatar'#243'w:'
          BoundLabel.Indent = 2
          BoundLabel.Font.Charset = EASTEUROPE_CHARSET
          BoundLabel.Font.Color = clWindowText
          BoundLabel.Font.Height = -11
          BoundLabel.Font.Name = 'Tahoma'
          BoundLabel.Font.Style = []
          BoundLabel.Layout = sclLeft
          BoundLabel.MaxWidth = 0
          BoundLabel.UseSkinColor = True
          SkinData.SkinSection = 'COMBOBOX'
          Style = csDropDownList
          Font.Charset = EASTEUROPE_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ItemIndex = 0
          ParentFont = False
          TabOrder = 0
          Text = 'Nigdy'
          OnChange = aSettingsChangedExecute
          Items.Strings = (
            'Nigdy'
            'Codziennie'
            'Co tydzie'#324
            'Co miesi'#261'c')
        end
      end
      object AvatarsStyleGroupBox: TsGroupBox
        Left = 8
        Top = 80
        Width = 299
        Height = 42
        Caption = 'Styl awatar'#243'w'
        TabOrder = 2
        SkinData.SkinSection = 'GROUPBOX'
        object AvatarsStyleLabel: TsLabel
          Left = 14
          Top = 20
          Width = 114
          Height = 13
          BiDiMode = bdLeftToRight
          Caption = 'Aktualnie u'#380'ywany styl:'
          ParentBiDiMode = False
          ParentFont = False
          Font.Charset = EASTEUROPE_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
        end
        object UsedAvatarsStyleLabel: TsLabelFX
          Left = 134
          Top = 20
          Width = 46
          Height = 14
          Caption = 'domy'#347'lny'
          Color = clBtnFace
          ParentColor = False
          ParentFont = False
          Font.Charset = EASTEUROPE_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          Angle = 0
          Kind.KindType = ktCustom
          Kind.Color = clGreen
          Shadow.BlurCount = 0
          Shadow.OffsetKeeper.LeftTop = 0
          Shadow.OffsetKeeper.RightBottom = 1
        end
        object EditAvatarsStyleLabel: TsLabel
          Left = 185
          Top = 20
          Width = 39
          Height = 13
          Cursor = crHandPoint
          Caption = '(edytuj)'
          ParentFont = False
          OnClick = EditAvatarsStyleLabelClick
          Font.Charset = EASTEUROPE_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsUnderline]
        end
        object AvatarStyleDefaultButton: TsButton
          Left = 119
          Top = 124
          Width = 89
          Height = 25
          Caption = 'Domy'#347'lny styl'
          TabOrder = 1
          TabStop = False
          OnClick = AvatarStyleDefaultButtonClick
          SkinData.SkinSection = 'BUTTON'
        end
        object AvatarStyleSaveButton: TsButton
          Left = 214
          Top = 124
          Width = 75
          Height = 25
          Caption = 'Zapisz'
          Enabled = False
          TabOrder = 2
          TabStop = False
          OnClick = AvatarStyleSaveButtonClick
          SkinData.SkinSection = 'BUTTON'
        end
        object AvatarsStyleMemo: TsMemo
          Left = 15
          Top = 45
          Width = 275
          Height = 73
          TabStop = False
          Font.Charset = EASTEUROPE_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          Lines.Strings = (
            '<span style="display: inline-block; padding: 2px 4px '
            '0px 1px; vertical-align: middle;">CC_AVATAR</span>')
          ParentFont = False
          TabOrder = 0
          OnChange = AvatarsStyleMemoChange
          Text = 
            '<span style="display: inline-block; padding: 2px 4px '#13#10'0px 1px; ' +
            'vertical-align: middle;">CC_AVATAR</span>'#13#10
          BoundLabel.Indent = 0
          BoundLabel.Font.Charset = DEFAULT_CHARSET
          BoundLabel.Font.Color = clWindowText
          BoundLabel.Font.Height = -11
          BoundLabel.Font.Name = 'Tahoma'
          BoundLabel.Font.Style = []
          BoundLabel.Layout = sclLeft
          BoundLabel.MaxWidth = 0
          BoundLabel.UseSkinColor = True
          SkinData.SkinSection = 'EDIT'
        end
      end
    end
    object HighlightMsgTabSheet: TsTabSheet
      Caption = 'Wyr'#243#380'nianie'
      SkinData.CustomColor = False
      SkinData.CustomFont = False
      object EraseHighlightMsgSpeedButton: TsSpeedButton
        Left = 17
        Top = 154
        Width = 23
        Height = 22
        Hint = 'Nowa fraza'
        Flat = True
        Glyph.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF000000
          00000000000000000000000000000000000000000000000000000000000B0000
          0022000000220000000B000000000000000000000000FFFFFF00FFFFFF000000
          000000000000000000000000000000000000000000000000000B7489AEAD90A9
          D3FF90A9D3FF7489AEAD0000000B0000000000000000FFFFFF00FFFFFF000000
          0000000000000000000000000000000000000000000B7489AEAD90A9D3FFF3F3
          FFFFF3F3FFFF90A9D3FF7489AEAD0000000B00000000FFFFFF00FFFFFF000000
          00000000000000000000000000000000000B7489AEAD90A9D3FFEBEBFFFFB3CC
          F3FF76A0E5FFE9E9FFFF90A9D3FF7489AEAD0000000BFFFFFF00FFFFFF000000
          000000000000000000000000000B7489AEAD90A9D3FFF3F3FFFFBCD5FBFF75A0
          E5FF749FE4FF7AA4E7FFEBEBFFFF90A9D3FF7F95BCA4FFFFFF00FFFFFF000000
          0000000000000000000B7489AEAD90A9D3FFFBFBFFFFC8DAFFFF7AA5EAFF7AA4
          E9FF77A3E7FF77A3E6FF7CA7EAFFFEFEFFFF90A9D3FFFFFFFF00FFFFFF000000
          00000000000B7489AEAD90A9D3FFFFFFFFFFD3D8FFFF80A7EBFF80A8EBFF7EA7
          EBFF7EA8EBFF7BA7EBFFC5D8FFFFFFFFFFFF90A9D3FFFFFFFF00FFFFFF000000
          000B798EB3A990A9D3FFFFFFFFFFE1E1FFFF88A8EBFF86A8EBFF85A8EBFF84A8
          EBFF83A7EBFFD1D8FFFFFFFFFFFF90A9D3FF90A9D396FFFFFF00FFFFFF007F95
          BCA490A9D3FFFFFFFFFFFFFFFFFFFFFFFFFFFBFBFFFF92AFEBFF8AA8EBFF89A7
          EBFFDDDDFFFFFFFFFFFF90A9D3FF90A9D39600000000FFFFFF00FFFFFF0090A9
          D3FFFFFFFFFF90A9D3FF90A9D3FF90A9D3FF90A9D3FFFFFFFFFF8FADEBFFEAEA
          FFFFFFFFFFFF90A9D3FF90A9D3960000000000000000FFFFFF00FFFFFF0090A9
          D3FFFFFFFFFF90A9D3FF000000000000000090A9D3FFFFFFFFFFF6F6FFFFFFFF
          FFFF90A9D3FF90A9D396000000000000000000000000FFFFFF00FFFFFF0090A9
          D3FFFFFFFFFF90A9D3FF0000002D0000002D90A9D3FFFFFFFFFFFFFFFFFF90A9
          D3FF90A9D39600000000000000000000000000000000FFFFFF00FFFFFF0090A9
          D3FFFFFFFFFF90A9D3FF90A9D3FF90A9D3FF90A9D3FFFFFFFFFF90A9D3FF90A9
          D3960000000000000000000000000000000000000000FFFFFF00FFFFFF0090A9
          D3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF90A9D3FF90A9D3960000
          00000000000000000000000000000000000000000000FFFFFF00FFFFFF0090A9
          D39690A9D3FF90A9D3FF90A9D3FF90A9D3FF90A9D3FF90A9D396000000000000
          00000000000000000000000000000000000000000000FFFFFF00}
        Layout = blGlyphBottom
        ParentShowHint = False
        ShowHint = True
        OnClick = EraseHighlightMsgSpeedButtonClick
        SkinData.SkinSection = 'SPEEDBUTTON'
        ShowCaption = False
      end
      object AddHighlightMsgsSpeedButton: TsSpeedButton
        Left = 251
        Top = 154
        Width = 23
        Height = 22
        Hint = 'Dodaj / Zmie'#324
        Flat = True
        Glyph.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF0040914D233C8E487D398B43DB35883FF331863BF32E83
          36DB2B80327D277E2F23FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF0048985753449552E640984FFF7CC18EFF95D0A5FF95CFA5FF77BD
          88FF358C41FF2B8033E6287E2F53FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00509E60534C9B5CF463B377FFA7DBB4FF86CC97FF65BB7CFF63B97BFF85CB
          97FFA4D9B3FF57A96AFF2B8033F4287E2F53FFFFFF00FFFFFF00FFFFFF0058A5
          6A2255A265E569B87CFFA7DBB1FF5FBB76FF5BB972FF58B76FFF58B46EFF57B4
          6EFF5AB673FFA4D9B2FF59A96BFF2B8133E5287E2F22FFFFFF00FFFFFF005CA8
          6F7E52AA67FFA9DDB3FF63C078FF5EBD70FF5FBB76FFFFFFFFFFFFFFFFFF58B7
          6FFF57B46DFF5BB673FFA5DAB3FF378E42FF2B81337EFFFFFF00FFFFFF0060AB
          74DB89CC97FF88D395FF6AC579FF62C06FFF54AA64FFFFFFFFFFFFFFFFFF58B7
          6FFF58B76FFF5AB871FF84CC96FF7ABD8CFF2F8438DBFFFFFF00FFFFFF0064AE
          79F6A8DDB2FF7CCF89FF74CC80FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFF58B76FFF66BD7CFF9BD4AAFF33873CF6FFFFFF00FFFFFF0068B1
          7DF6B5E2BDFF8AD596FF79C985FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFF58B76FFF68C07DFF9CD4A9FF378A41F6FFFFFF00FFFFFF006CB4
          82DBABDDB5FFA5DFAEFF80CB8BFF7BC985FF6DBC78FFFFFFFFFFFFFFFFFF5AAB
          69FF5FBB76FF5BB972FF8AD198FF7FC491FF3B8D46DBFFFFFF00FFFFFF006FB7
          867E84C796FFD2EED7FF94D99FFF89D393FF7EC888FFFFFFFFFFFFFFFFFF78CD
          84FF6AC27BFF6EC77DFFABDFB4FF449D56FF3F904B7EFFFFFF00FFFFFF0072B9
          89226FB786E5A9DAB6FFD8F1DCFF91D89CFF87CD92FF83CC8DFF8AD495FF89D4
          94FF82D28DFFAEE0B6FF6AB87CFF479755E543935022FFFFFF00FFFFFF00FFFF
          FF0072BA8A536FB786F4AEDCBAFFDCF2E0FFB5E4BCFF9ADBA4FF95D99FFFA4DF
          AEFFBFE8C4FF78C189FF4F9D5EF44B9A5A53FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF0072BA8A5370B786E693CEA3FFC2E6CBFFCFEBD4FFC9E9CEFFAEDD
          B7FF6CB87EFF57A468E653A06453FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF0073BA8A2370B8877D6DB583DB69B27FF366B07BF362AD
          76DB5FAA727D5BA76D23FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00}
        ParentShowHint = False
        ShowHint = True
        OnClick = AddHighlightMsgsSpeedButtonClick
        SkinData.SkinSection = 'SPEEDBUTTON'
        ShowCaption = False
      end
      object RemoveHighlightMsgSpeedButton: TsSpeedButton
        Left = 274
        Top = 154
        Width = 23
        Height = 22
        Hint = 'Usu'#324
        Enabled = False
        Flat = True
        Glyph.Data = {
          36040000424D3604000000000000360000002800000010000000100000000100
          2000000000000004000000000000000000000000000000000000FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF003E53C223394FC17D3752BDDB3450BCF32F4ABBF32D4D
          B7DB2A4BB67D2647B423FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF004557C8534154C5E63B51CCFF7479E8FF8E91EEFF8E91EEFF7077
          E4FF324CC0FF2A49B6E6284AB453FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF004C59CD534858CBF45B64E0FFA0A5F5FF7D85EFFF5A62E9FF585CE7FF7C83
          EEFF9D9FF4FF505CD7FF2A49B6F4284AB453FFFFFF00FFFFFF00FFFFFF00535E
          D2225260CFE5606AE3FFA0ABF5FF535EECFF4F5BEAFF4C58E9FF4D58E6FF4B55
          E6FF4F55E6FF9DA1F4FF535FD6FF2949B7E5284AB422FFFFFF00FFFFFF00575F
          D47E4A55DBFFA1AAF6FF5563F0FF5165EEFF4C58E9FF4C58E9FF4C58E9FF4C58
          E9FF4B57E6FF5159E6FF9EA2F5FF334FC3FF2949B77EFFFFFF00FFFFFF005B61
          D7DB808BEEFF7D90F7FF5C72F3FF4C58E9FF4C58E9FF4C58E9FF4C58E9FF4C58
          E9FF4C58E9FF4E5AE9FF7A82F0FF747AE2FF2D4AB9DBFFFFFF00FFFFFF005E62
          DAF6A0AAF7FF6F85F8FF6781F6FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFF4C58E9FF5B65EAFF959BF1FF314FBBF6FFFFFF00FFFFFF006368
          DBF6AEB8F9FF7E92FAFF6F84F0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
          FFFFFFFFFFFF4C58E9FF5D69EEFF959CF1FF354EBDF6FFFFFF00FFFFFF006669
          DEDBA4AEF5FF9CAAFAFF768BF0FF535EECFF535EECFF535EECFF535EECFF535E
          ECFF535EECFF6276F2FF808DF4FF777EE9FF3952BFDBFFFFFF00FFFFFF006968
          E07E7C82EAFFCDD4FCFF8A9CFAFF7D92F7FF7489EEFF6B83F6FF6B83F6FF6B83
          F6FF6B83F6FF6278F3FFA3AEF8FF3D4ED0FF3D53C17EFFFFFF00FFFFFF006B6B
          E1226968E0E5A2A6F3FFD4DBFDFF8699FAFF7E90F0FF798DF1FF7E93F8FF7D91
          F9FF758BF8FFA7B5F8FF626DE3FF4456C7E54055C422FFFFFF00FFFFFF00FFFF
          FF006C6BE3536968E0F4A9ACF2FFD8DCFDFFADB9FAFF90A2FAFF8A9CFAFF9BA8
          FBFFB9C7FCFF6F7AE9FF4B5ACCF44757CA53FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF006C6BE3536969DFE68D92EDFFBDC2F8FFCCD3F9FFC3CBF9FFA9B3
          F4FF656FE2FF525DD1E64F5CCE53FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF006C6CE2236A69E17D6769DDDB6263DCF36063DAF35C62
          D9DB5A62D67D5761D423FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
          FF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFFFF00}
        ParentShowHint = False
        ShowHint = True
        OnClick = RemoveHighlightMsgSpeedButtonClick
        SkinData.SkinSection = 'SPEEDBUTTON'
        ShowCaption = False
      end
      object HighlightMsgColorSelect: TsColorSelect
        Left = 222
        Top = 154
        Width = 23
        Height = 22
        Hint = 'Wybierz kolor'
        Glyph.Data = {
          BA030000424DBA0300000000000036000000280000000F0000000F0000000100
          2000000000008403000000000000000000000000000000000000FF00FF000000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFFFF00FF000000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFFFF00FF000000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000
          FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFF0000FFFFFF00FF00}
        ParentShowHint = False
        ShowHint = True
        SkinData.SkinSection = 'SPEEDBUTTON'
        OnChange = HighlightMsgColorSelectChange
        ColorValue = clRed
        StandardDlg = True
      end
      object HighlightMsgModeLabel: TsLabel
        Left = 31
        Top = 230
        Width = 243
        Height = 13
        Caption = 'Dzia'#322'a tylko przy wyr'#243#380'nianiu tag'#243'w/u'#380'ytkownik'#243'w'
        Enabled = False
        Visible = False
      end
      object HighlightMsgCheckBox: TsCheckBox
        Left = 6
        Top = 6
        Width = 240
        Height = 20
        Caption = 'Wyr'#243#380'niaj wiadomo'#347'ci zawieraj'#261'ce dan'#261' fraz'#281
        TabOrder = 0
        OnClick = HighlightMsgCheckBoxClick
        SkinData.SkinSection = 'CHECKBOX'
        ImgChecked = 0
        ImgUnchecked = 0
      end
      object HighlightMsgListView: TsListView
        Left = 17
        Top = 36
        Width = 280
        Height = 113
        BoundLabel.Indent = 0
        BoundLabel.Font.Charset = DEFAULT_CHARSET
        BoundLabel.Font.Color = clWindowText
        BoundLabel.Font.Height = -11
        BoundLabel.Font.Name = 'Tahoma'
        BoundLabel.Font.Style = []
        BoundLabel.Layout = sclLeft
        BoundLabel.MaxWidth = 0
        BoundLabel.UseSkinColor = True
        SkinData.SkinSection = 'EDIT'
        Columns = <
          item
            Caption = 'Fraza'
            MaxWidth = 186
            MinWidth = 186
            Width = 186
          end
          item
            Caption = 'Kolor'
            MaxWidth = 60
            MinWidth = 60
            Width = 60
          end>
        RowSelect = True
        TabOrder = 1
        ViewStyle = vsReport
        OnEdited = HighlightMsgListViewEdited
        OnKeyDown = HighlightMsgListViewKeyDown
        OnSelectItem = HighlightMsgListViewSelectItem
      end
      object ColorHighlightMsgEdit: TsEdit
        Left = 164
        Top = 155
        Width = 56
        Height = 21
        MaxLength = 7
        TabOrder = 3
        Text = '#FF0000'
        OnChange = ColorHighlightMsgEditChange
        SkinData.SkinSection = 'EDIT'
        BoundLabel.Indent = 0
        BoundLabel.Font.Charset = DEFAULT_CHARSET
        BoundLabel.Font.Color = clWindowText
        BoundLabel.Font.Height = -11
        BoundLabel.Font.Name = 'Tahoma'
        BoundLabel.Font.Style = []
        BoundLabel.Layout = sclLeft
        BoundLabel.MaxWidth = 0
        BoundLabel.UseSkinColor = True
      end
      object ItemHighlightMsgEdit: TsEdit
        Left = 46
        Top = 155
        Width = 110
        Height = 21
        TabOrder = 2
        SkinData.SkinSection = 'EDIT'
        BoundLabel.Indent = 0
        BoundLabel.Font.Charset = DEFAULT_CHARSET
        BoundLabel.Font.Color = clWindowText
        BoundLabel.Font.Height = -11
        BoundLabel.Font.Name = 'Tahoma'
        BoundLabel.Font.Style = []
        BoundLabel.Layout = sclLeft
        BoundLabel.MaxWidth = 0
        BoundLabel.UseSkinColor = True
      end
      object HighlightMsgModeComboBox: TsComboBox
        Left = 17
        Top = 203
        Width = 186
        Height = 22
        Alignment = taLeftJustify
        BoundLabel.Active = True
        BoundLabel.Caption = 'Spos'#243'b wyr'#243#380'niania wiadomo'#347'ci:'
        BoundLabel.Indent = 4
        BoundLabel.Font.Charset = EASTEUROPE_CHARSET
        BoundLabel.Font.Color = clWindowText
        BoundLabel.Font.Height = -11
        BoundLabel.Font.Name = 'Tahoma'
        BoundLabel.Font.Style = []
        BoundLabel.Layout = sclTopLeft
        BoundLabel.MaxWidth = 0
        BoundLabel.UseSkinColor = True
        SkinData.SkinSection = 'COMBOBOX'
        Style = csOwnerDrawFixed
        ItemIndex = 0
        TabOrder = 4
        Text = 'Zmieniaj kolor tekstu'
        OnChange = HighlightMsgModeComboBoxChange
        Items.Strings = (
          'Zmieniaj kolor tekstu'
          'Zmieniaj kolor tekstu i odno'#347'nik'#243'w'
          'Zmieniaj kolor szukanej frazy'
          'Zmieniaj kolor pola wiadomo'#347'ci')
      end
    end
  end
  object FileMemo: TMemo
    Left = 0
    Top = 0
    Width = 0
    Height = 0
    Lines.Strings = (
      'FileMemo')
    TabOrder = 4
    Visible = False
  end
  object ActionList: TActionList
    Left = 96
    Top = 264
    object aLoadSettings: TAction
      Caption = 'aLoadSettings'
      OnExecute = aLoadSettingsExecute
    end
    object aSaveSettings: TAction
      Caption = 'aSaveSettings'
      OnExecute = aSaveSettingsExecute
    end
    object aSettingsChanged: TAction
      Caption = 'aSettingsChanged'
      OnExecute = aSettingsChangedExecute
    end
    object aExit: TAction
      Caption = 'aExit'
      ShortCut = 27
      OnExecute = aExitExecute
    end
    object aForceDisconnect: TAction
      Caption = 'aForceDisconnect'
      OnExecute = aForceDisconnectExecute
    end
  end
  object ManualAvatarsUpdateThread: TIdThreadComponent
    Active = False
    Loop = False
    Priority = tpNormal
    StopMode = smTerminate
    OnRun = ManualAvatarsUpdateThreadRun
    Left = 96
    Top = 296
  end
  object AutoAvatarsUpdateThread: TIdThreadComponent
    Active = False
    Loop = False
    Priority = tpNormal
    StopMode = smTerminate
    OnRun = AutoAvatarsUpdateThreadRun
    Left = 128
    Top = 296
  end
  object AUIdHTTP: TIdHTTP
    OnWork = AUIdHTTPWork
    OnWorkBegin = AUIdHTTPWorkBegin
    OnWorkEnd = AUIdHTTPWorkEnd
    AllowCookies = False
    HandleRedirects = True
    ProxyParams.BasicAuthentication = False
    ProxyParams.ProxyPort = 0
    Request.ContentLength = -1
    Request.ContentRangeEnd = -1
    Request.ContentRangeStart = -1
    Request.ContentRangeInstanceLength = -1
    Request.CustomHeaders.Strings = (
      'X-Blip-API: 0.02')
    Request.Pragma = 'no-cache'
    Request.Accept = '*/*'
    Request.BasicAuthentication = False
    Request.UserAgent = 'AQQ IM Plugin: Blabler (+http://beherit.pl)'
    Request.Ranges.Units = 'bytes'
    Request.Ranges = <>
    HTTPOptions = [hoForceEncodeParams]
    Left = 32
    Top = 296
  end
  object AnimateTimer: TTimer
    Enabled = False
    Interval = 15
    OnTimer = AnimateTimerTimer
    Top = 264
  end
  object sSkinManager: TsSkinManager
    ExtendedBorders = True
    AnimEffects.FormShow.Time = 200
    Active = False
    InternalSkins = <>
    MenuSupport.IcoLineSkin = 'ICOLINE'
    MenuSupport.ExtraLineFont.Charset = DEFAULT_CHARSET
    MenuSupport.ExtraLineFont.Color = clWindowText
    MenuSupport.ExtraLineFont.Height = -11
    MenuSupport.ExtraLineFont.Name = 'Tahoma'
    MenuSupport.ExtraLineFont.Style = []
    SkinDirectory = 'c:\Skins'
    SkinInfo = 'N/A'
    ThirdParty.ThirdEdits = ' '
    ThirdParty.ThirdButtons = 'TButton'
    ThirdParty.ThirdBitBtns = ' '
    ThirdParty.ThirdCheckBoxes = ' '
    ThirdParty.ThirdGroupBoxes = ' '
    ThirdParty.ThirdListViews = ' '
    ThirdParty.ThirdPanels = ' '
    ThirdParty.ThirdGrids = ' '
    ThirdParty.ThirdTreeViews = ' '
    ThirdParty.ThirdComboBoxes = ' '
    ThirdParty.ThirdWWEdits = ' '
    ThirdParty.ThirdVirtualTrees = ' '
    ThirdParty.ThirdGridEh = ' '
    ThirdParty.ThirdPageControl = ' '
    ThirdParty.ThirdTabControl = ' '
    ThirdParty.ThirdToolBar = ' '
    ThirdParty.ThirdStatusBar = ' '
    ThirdParty.ThirdSpeedButton = ' '
    ThirdParty.ThirdScrollControl = ' '
    ThirdParty.ThirdUpDown = ' '
    ThirdParty.ThirdScrollBar = ' '
    Left = 160
    Top = 264
  end
  object sSkinProvider: TsSkinProvider
    AddedTitle.Font.Charset = DEFAULT_CHARSET
    AddedTitle.Font.Color = clNone
    AddedTitle.Font.Height = -11
    AddedTitle.Font.Name = 'Tahoma'
    AddedTitle.Font.Style = []
    SkinData.SkinSection = 'FORM'
    TitleButtons = <>
    Left = 128
    Top = 264
  end
  object IdHTTP: TIdHTTP
    OnWork = IdHTTPWork
    OnWorkBegin = IdHTTPWorkBegin
    OnWorkEnd = IdHTTPWorkEnd
    AllowCookies = False
    HandleRedirects = True
    ProxyParams.BasicAuthentication = False
    ProxyParams.ProxyPort = 0
    Request.ContentLength = -1
    Request.ContentRangeEnd = -1
    Request.ContentRangeStart = -1
    Request.ContentRangeInstanceLength = -1
    Request.CustomHeaders.Strings = (
      'X-Blip-API: 0.02')
    Request.Pragma = 'no-cache'
    Request.Accept = '*/*'
    Request.BasicAuthentication = False
    Request.UserAgent = 'AQQ IM Plugin: Blabler (+http://beherit.pl)'
    Request.Ranges.Units = 'bytes'
    Request.Ranges = <>
    HTTPOptions = [hoForceEncodeParams]
    Top = 296
  end
  object AUIdHTTPTimer: TTimer
    Enabled = False
    Interval = 10000
    OnTimer = AUIdHTTPTimerTimer
    Left = 64
    Top = 264
  end
  object AvatarsIdHTTPTimer: TTimer
    Enabled = False
    Interval = 10000
    OnTimer = AvatarsIdHTTPTimerTimer
    Left = 32
    Top = 264
  end
  object GetAvatarsThread: TIdThreadComponent
    Active = False
    Loop = True
    Priority = tpNormal
    StopMode = smTerminate
    OnRun = GetAvatarsThreadRun
    Left = 64
    Top = 296
  end
end
