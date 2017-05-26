RunLuaMenuDestroyKeys = { KEY_EQUAL, KEY_BACKSPACE }

RunLuaMenuToggleKeys = { KEY_EQUAL, KEY_LBRACKET }

RunLuaMenuToggle = true

RunLuaMenuToggleDelay = 0.5
RunLuaMenuToggleExecuteLastTime = nil

function thinkRunLuaMenuDestroy()
	local destroy = true
	for k, v in ipairs(RunLuaMenuDestroyKeys) do
		if not input.IsKeyDown(v) then
			destroy = false
			break
		end
	end
	if destroy == true then
		if RunLuaMenuDestroy ~= nil then
			hook.Remove('Think', 'RunLuaMenuDestroy')
			hook.Remove('Think', 'RunLuaMenuHide')
			RunLuaMenuDestroyKeys = nil
			RunLuaMenuToggleKeys = nil
			RunLuaMenuToggle = nil
			RunLuaMenuToggleDelay = nil
			RunLuaMenuToggleExecuteLastTime = nil
			menuRunLuaMenuPosX = nil
			menuRunLuaMenuPosY = nil
			if textRunLuaMenu ~= nil and buttonRunLuaMenu ~= nil and menuRunLuaMenu ~= nil then
				textRunLuaMenu:Remove()
				textRunLuaMenu = nil
				buttonRunLuaMenu:Remove()
				buttonRunLuaMenu = nil
				menuRunLuaMenu:Remove()
				menuRunLuaMenu = nil
			end
			--[[ gaceio start --]]
			gaceioList = nil
			gaceioRead = nil
			gaceioWrite = nil
			gaceioDelete = nil
			gaceioIsDir = nil
			gaceioExists = nil
			gaceioCreateDir = nil
			gaceioTime = nil
			gaceioSize = nil
			gaceioCRC = nil
			--[[ gaceio end --]]
			RunLuaMenu = nil
			IsClientLuaValid = nil
			LocalPlayerIsValid = nil
			LocalPlayerAlive = nil
			netStart = nil
			netSendToServer = nil
			netWriteAngle = nil
			netWriteBit = nil
			netWriteBool = nil
			netWriteColor = nil
			netWriteData = nil
			netWriteDouble = nil
			netWriteFloat = nil
			netWriteInt = nil
			netWriteNormal = nil
			netWriteString = nil
			netWriteTable = nil
			netWriteUInt = nil
			netWriteVector = nil
			local RunLuaMenuDestroyLocal = RunLuaMenuDestroy
			RunLuaMenuDestroy = nil
			RunLuaMenuDestroyLocal()
		end	
	end
end

function thinkRunLuaMenuHide()
	local toggle = true
	for k, v in ipairs(RunLuaMenuToggleKeys) do
		if not input.IsKeyDown(v) then
			toggle = false
			break
		end
	end
	if toggle == true then
		if RunLuaMenuToggleExecuteLastTime == nil or CurTime() - RunLuaMenuToggleExecuteLastTime >= RunLuaMenuToggleDelay then
			RunLuaMenuToggle = !RunLuaMenuToggle
			RunLuaMenuToggleExecuteLastTime = CurTime()
		end
	end
	if IsClientLuaValid ~= nil and IsClientLuaValid() then
		if textRunLuaMenu ~= nil and buttonRunLuaMenu ~= nil and menuRunLuaMenu ~= nil then
			textRunLuaMenu:Remove()
			textRunLuaMenu = nil
			buttonRunLuaMenu:Remove()
			buttonRunLuaMenu = nil
			local posX, posY = menuRunLuaMenu:GetPos()
			menuRunLuaMenuPosX = posX
			menuRunLuaMenuPosY = posY
			menuRunLuaMenu:Remove()
			menuRunLuaMenu = nil
		end
	else
		if GetLoadStatus() == nil then
			if RunLuaMenuToggle == true then
				if textRunLuaMenu == nil and buttonRunLuaMenu == nil and menuRunLuaMenu == nil then
					menuRunLuaMenu = vgui.Create('DFrame')
					menuRunLuaMenu:SetSize(ScrW() * 0.3, ScrH() * 0.22)
					if menuRunLuaMenuPosX ~= nil and menuRunLuaMenuPosY ~= nil then
						menuRunLuaMenu:SetPos(menuRunLuaMenuPosX, menuRunLuaMenuPosY)
					else
						menuRunLuaMenu:Center()
					end
					menuRunLuaMenu:SetDraggable(true)
					menuRunLuaMenu:ShowCloseButton(false)
					menuRunLuaMenu:SetTitle('RunLuaMenu Panel')
					menuRunLuaMenu:MakePopup()

					textRunLuaMenu = vgui.Create( 'DTextEntry', menuRunLuaMenu )
					textRunLuaMenu:SetPos( ScrW() * 0, ScrH() * 0.02 )
					textRunLuaMenu:SetSize( ScrW() * 0.3, ScrH() * 0.1 )

					buttonRunLuaMenu = vgui.Create( 'DButton', menuRunLuaMenu )
					buttonRunLuaMenu:SetText( 'Run Lua Menu' )
					buttonRunLuaMenu:SetPos( ScrW() * 0, ScrH() * 0.12 )
					buttonRunLuaMenu:SetSize( ScrW() * 0.3, ScrH() * 0.1 )
					buttonRunLuaMenu.DoClick = function()
						if RunLuaMenu ~= nil then
							RunLuaMenu(textRunLuaMenu:GetValue())
							textRunLuaMenu:SetText('')
						end
					end
				end
			else
				if textRunLuaMenu ~= nil and buttonRunLuaMenu ~= nil and menuRunLuaMenu ~= nil then
					textRunLuaMenu:Remove()
					textRunLuaMenu = nil
					buttonRunLuaMenu:Remove()
					buttonRunLuaMenu = nil
					local posX, posY = menuRunLuaMenu:GetPos()
					menuRunLuaMenuPosX = posX
					menuRunLuaMenuPosY = posY
					menuRunLuaMenu:Remove()
					menuRunLuaMenu = nil
				end
			end
		else
			if textRunLuaMenu ~= nil and buttonRunLuaMenu ~= nil and menuRunLuaMenu ~= nil then
				textRunLuaMenu:Remove()
				textRunLuaMenu = nil
				buttonRunLuaMenu:Remove()
				buttonRunLuaMenu = nil
				local posX, posY = menuRunLuaMenu:GetPos()
				menuRunLuaMenuPosX = posX
				menuRunLuaMenuPosY = posY
				menuRunLuaMenu:Remove()
				menuRunLuaMenu = nil
			end
		end
	end
end

hook.Add('Think', 'RunLuaMenuDestroy', thinkRunLuaMenuDestroy)

hook.Add('Think', 'RunLuaMenuHide', thinkRunLuaMenuHide)