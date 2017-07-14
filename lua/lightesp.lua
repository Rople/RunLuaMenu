lightesp = {}

lightesp.folder = ''
lightesp.configFile = ''

lightesp.config = {}

lightesp.configRefreshDelay = 1
lightesp.configRefreshExecuteLastTime = nil

lightesp.destroyKeys = { KEY_EQUAL, KEY_BACKSPACE }

if lightesp.folder == '' or lightesp.configFile == '' then
	return
end

function lightesp.IsValidColor(obj)
	if type(obj) ~= 'table' then
		return false
	end
	local r, g, b, a
	local count = table.Count(obj)
	if count ~= 4 then
		return false
	end
	for k, v in pairs(obj) do
		if type(k) ~= 'string' then
			return false
		end
		if type(v) ~= 'number' then
			return false
		end
		if v < 0 or v > 255 then
			return false
		end
		if k == 'r' then
			r = true
		end
		if k == 'g' then
			g = true
		end
		if k == 'b' then
			b = true
		end
		if k == 'a' then
			a = true
		end
	end
	if r ~= nil and g ~= nil and b ~= nil and a ~= nil then
		return true
	end
	return false
end

function lightesp.getConfig()
	if not gaceioIsDir( lightesp.folder) then
		gaceioCreateDir( lightesp.folder)
	end
	if not gaceioExists( lightesp.folder .. '/' .. lightesp.configFile) then
		gaceioWrite( lightesp.folder .. '/' .. lightesp.configFile, '{ }')
	end
	local content = gaceioRead( lightesp.folder .. '/' .. lightesp.configFile)
	local t = util.JSONToTable(content)
	if t ~= nil then
		local err = false
		for k, v in pairs(t) do
			if type(k) ~= 'string' then
				err = true
				break
			end
			if k == 'entities' then
				if type(v) ~= 'table' then
					err = true
					break
				end
				for k2, v2 in pairs(v) do
					if type(k2) ~= 'string' then
						err = true
						break
					end
					if lightesp.IsValidColor(v2) == false then
						err = true
						break
					end
				end
			elseif k == 'nickEnable' then
				if type(v) ~= 'boolean' then
					err = true
					break
				end
			elseif k == 'nickText' then
				if type(v) ~= 'string' then
					err = true
					break
				end
			elseif k == 'nickFont' then
				if type(v) ~= 'string' then
					err = true
					break
				end
			elseif k == 'nickXOffset' then
				if type(v) ~= 'number' then
					err = true
					break
				end
			elseif k == 'nickZOffset' then
				if type(v) ~= 'number' then
					err = true
					break
				end
			elseif k == 'nickColor' then
				if lightesp.IsValidColor(v) == false then
					err = true
					break
				end
			elseif k == 'nickXAlign' then
				if type(v) ~= 'number' then
					err = true
					break
				end
				if v < 0 or v > 4 then
					err = true
					break
				end
			elseif k == 'healthEnable' then
				if type(v) ~= 'boolean' then
					err = true
					break
				end
			elseif k == 'healthText' then
				if type(v) ~= 'string' then
					err = true
					break
				end
			elseif k == 'healthFont' then
				if type(v) ~= 'string' then
					err = true
					break
				end
			elseif k == 'healthXOffset' then
				if type(v) ~= 'number' then
					err = true
					break
				end
			elseif k == 'healthZOffset' then
				if type(v) ~= 'number' then
					err = true
					break
				end
			elseif k == 'healthColor' then
				if lightesp.IsValidColor(v) == false then
					err = true
					break
				end
			elseif k == 'healthXAlign' then
				if type(v) ~= 'number' then
					err = true
					break
				end
				if v < 0 or v > 4 then
					err = true
					break
				end
			elseif k == 'crosshairEnable' then
				if type(v) ~= 'boolean' then
					err = true
					break
				end
			elseif k == 'crosshairGap' then
				if type(v) ~= 'number' then
					err = true
					break
				end
				if v < 0 then
					err = true
					break
				end
			elseif k == 'crosshairLength' then
				if type(v) ~= 'number' then
					err = true
					break
				end
				if v < 1 then
					err = true
					break
				end
			elseif k == 'crosshairColor' then
				if lightesp.IsValidColor(v) == false then
					err = true
					break
				end
			elseif k == 'spectatorsEnable' then
				if type(v) ~= 'boolean' then
					err = true
					break
				end
			elseif k == 'spectatorsSeparator' then
				if type(v) ~= 'string' then
					err = true
					break
				end
			elseif k == 'spectatorsText' then
				if type(v) ~= 'string' then
					err = true
					break
				end
			elseif k == 'spectatorsFont' then
				if type(v) ~= 'string' then
					err = true
					break
				end
			elseif k == 'spectatorsXPosition' then
				if type(v) ~= 'number' then
					err = true
					break
				end
			elseif k == 'spectatorsYPosition' then
				if type(v) ~= 'number' then
					err = true
					break
				end
			elseif k == 'spectatorsColor' then
				if lightesp.IsValidColor(v) == false then
					err = true
					break
				end
			elseif k == 'spectatorsXAlign' then
				if type(v) ~= 'number' then
					err = true
					break
				end
			elseif k == 'spectatorsMaxCharsPerLine' then
				if type(v) ~= 'number' then
					err = true
					break
				end
				if v < 1 then
					err = true
					break
				end
			elseif k == 'spectatorsMaxLines' then
				if type(v) ~= 'number' then
					err = true
					break
				end
				if v < 1 then
					err = true
					break
				end
			elseif k == 'spectatorsYSpace' then
				if type(v) ~= 'number' then
					err = true
					break
				end
			else
				err = true
				break
			end
		end
		if not err then
			return t
		end
	end
	return nil
end

function lightesp.setConfig(newConfig)
	lightesp.config = newConfig
end

function lightesp.getBoxOnScreen( entIndex)
	local OBBMins, OBBMaxs = EntityOBBMins(entIndex), EntityOBBMaxs(entIndex)
	local corners = {
		Vector( OBBMins.x, OBBMins.y, OBBMins.z ),
		Vector( OBBMins.x, OBBMins.y, OBBMaxs.z ),
		Vector( OBBMins.x, OBBMaxs.y, OBBMins.z ),
		Vector( OBBMins.x, OBBMaxs.y, OBBMaxs.z ),
		Vector( OBBMaxs.x, OBBMins.y, OBBMins.z ),
		Vector( OBBMaxs.x, OBBMins.y, OBBMaxs.z ),
		Vector( OBBMaxs.x, OBBMaxs.y, OBBMins.z ),
		Vector( OBBMaxs.x, OBBMaxs.y, OBBMaxs.z )
	}
	 
	local xMin, yMin, xMax, yMax
	for _, corner in pairs( corners ) do
		local cornerWorld = EntityLocalToWorld(entIndex, corner)
		local cornerOnScreen = VectorToScreen(cornerWorld)
		if xMin != nil then
			xMin, yMin, xMax, yMax = math.min( xMin, cornerOnScreen.x ), math.min( yMin, cornerOnScreen.y ), math.max( xMax, cornerOnScreen.x ), math.max( yMax, cornerOnScreen.y )
		else
			xMin, yMin, xMax, yMax = cornerOnScreen.x, cornerOnScreen.y, cornerOnScreen.x, cornerOnScreen.y
		end
	end
	 
	return xMin, yMin, xMax, yMax
end

function lightesp.drawoverlay()
	if lightesp.configRefreshExecuteLastTime == nil or CurTime() - lightesp.configRefreshExecuteLastTime >= lightesp.configRefreshDelay then
		local config = lightesp.getConfig()
		lightesp.setConfig(config)
		lightesp.configRefreshExecuteLastTime = CurTime()
	end

	if not IsClientLuaValid() then
		return
	end
	
	if lightesp.config == nil then
		return
	end
	
	local crosshairEnable = lightesp.config.crosshairEnable
	if crosshairEnable == true then
		local crosshairGap = lightesp.config.crosshairGap
		local crosshairLength = lightesp.config.crosshairLength
		local crosshairColor = lightesp.config.crosshairColor
	
		local xCenter = ScrW() / 2
		local yCenter = ScrH() / 2
	 
		surface.SetDrawColor(crosshairColor.r, crosshairColor.g, crosshairColor.b, crosshairColor.a)
		surface.DrawLine( xCenter - crosshairGap - crosshairLength, yCenter, xCenter - crosshairGap, yCenter )
		surface.DrawLine( xCenter + crosshairGap + crosshairLength, yCenter, xCenter + crosshairGap, yCenter )
		surface.DrawLine( xCenter, yCenter - crosshairGap - crosshairLength, xCenter, yCenter - crosshairGap )
		surface.DrawLine( xCenter, yCenter + crosshairGap + crosshairLength, xCenter, yCenter + crosshairGap )
	end
	
	for entityClass, espColor in pairs( lightesp.config.entities ) do
		for _, entIndex in pairs( entsFindByClass( entityClass ) ) do
			if entityClass ~= 'player' or (entIndex ~= LocalPlayerEntIndex() and PlayerAlive(entIndex) == true and PlayerGetObserverMode(entIndex) == 0) then
				local xMin, yMin, xMax, yMax = lightesp.getBoxOnScreen(entIndex)
				
				surface.SetDrawColor(espColor.r, espColor.g, espColor.b, espColor.a)
				surface.DrawLine( xMin, yMin, xMax, yMin )
				surface.DrawLine( xMax, yMin, xMax, yMax )
				surface.DrawLine( xMax, yMax, xMin, yMax )
				surface.DrawLine( xMin, yMax, xMin, yMin )
			end
		end
	end
	
	local nickEnable = lightesp.config.nickEnable
	local healthEnable = lightesp.config.healthEnable
	if nickEnable == true or healthEnable == true then
		for _, entIndex in ipairs( entsFindByClass( 'player' ) ) do
			if entIndex ~= LocalPlayerEntIndex() and PlayerAlive(entIndex) == true and PlayerGetObserverMode(entIndex) == 0 then
				local OBBHeight = EntityOBBMaxs(entIndex).z - EntityOBBMins(entIndex).z
				local yCenter = OBBHeight / 2
				
				local entPos = EntityGetPos(entIndex)
				
				if nickEnable == true then
					local nickText = lightesp.config.nickText
					local nickFont = lightesp.config.nickFont
					local nickXOffset = lightesp.config.nickXOffset
					local nickZOffset = lightesp.config.nickZOffset
					local nickColor = lightesp.config.nickColor
					local nickXAlign = lightesp.config.nickXAlign
					
					local nick = PlayerNick(entIndex)
					
					local textPos = VectorToScreen(Vector(entPos.x + nickXOffset, entPos.y, entPos.z + yCenter + nickZOffset))
					draw.DrawText(string.Replace(nickText, '<nick>', nick), nickFont, textPos.x, textPos.y, nickColor, nickXAlign)
				end
				
				
				if healthEnable == true then
					local healthText = lightesp.config.healthText
					local healthFont = lightesp.config.healthFont
					local healthXOffset = lightesp.config.healthXOffset
					local healthZOffset = lightesp.config.healthZOffset
					local healthColor = lightesp.config.healthColor
					local healthXAlign = lightesp.config.healthXAlign	

					local health = EntityHealth(entIndex)
					local maxHealth = EntityGetMaxHealth(entIndex)					
					
					local textPos = VectorToScreen(Vector(entPos.x + healthXOffset, entPos.y, entPos.z + yCenter + healthZOffset))
					draw.DrawText(string.Replace(string.Replace(healthText, '<health>', health), '<maxHealth>', maxHealth), healthFont, textPos.x, textPos.y, healthColor, healthXAlign)
				end
			end
		end
	end
	
	local spectatorsEnable = lightesp.config.spectatorsEnable
	if spectatorsEnable == true then
		local spectatorsTable = {}
		for _, entIndex in ipairs( entsFindByClass( 'player' ) ) do
			if entIndex ~= LocalPlayerEntIndex() then
				local target = PlayerGetObserverTarget(entIndex)
				if target ~= nil and target == LocalPlayerEntIndex() then
					table.insert( spectatorsTable, entIndex )
				end
			end
		end
		
		local spectatorsSeparator = lightesp.config.spectatorsSeparator
		local spectators = ''
		local count = table.Count(spectatorsTable)
		for index, entIndex in ipairs( spectatorsTable ) do
			local nick = PlayerNick(entIndex)
			spectators = spectators .. nick
			if count ~= index then
				spectators = spectators .. spectatorsSeparator
			end
		end
		
		local spectatorsText = lightesp.config.spectatorsText
		local spectatorsFont = lightesp.config.spectatorsFont
		local spectatorsXPosition = lightesp.config.spectatorsXPosition
		local spectatorsYPosition = lightesp.config.spectatorsYPosition
		local spectatorsColor = lightesp.config.spectatorsColor
		local spectatorsXAlign = lightesp.config.spectatorsXAlign
		local spectatorsMaxCharsPerLine = lightesp.config.spectatorsMaxCharsPerLine
		local spectatorsMaxLines = lightesp.config.spectatorsMaxLines
		local spectatorsYSpace = lightesp.config.spectatorsYSpace
		
		local spectatorsFinalText = string.Replace(spectatorsText, '<spectators>', spectators)
		
		local spectatorsFinalTextParts = {}
		
		local spectatorsFinalTextLength = string.len(spectatorsFinalText)
		if spectatorsFinalTextLength == 0 then
			return
		end
		local startIndex = 1
		while true do
			local endIndex = math.min(startIndex + spectatorsMaxCharsPerLine - 1, spectatorsFinalTextLength)
			local spectatorsTextPart = string.sub(spectatorsFinalText, startIndex, endIndex)
			table.insert(spectatorsFinalTextParts, spectatorsTextPart)
			if endIndex == spectatorsFinalTextLength then
				break
			end
			startIndex = startIndex + spectatorsMaxCharsPerLine
		end
		
		for i=1,spectatorsMaxLines do
			local textPosX = (ScrW() * spectatorsXPosition)
			local textPosY = (ScrH() * spectatorsYPosition) + (ScrH() * spectatorsYSpace * (i - 1))
			if spectatorsFinalTextParts[i] ~= nil then
				draw.DrawText(spectatorsFinalTextParts[i], spectatorsFont, textPosX, textPosY, spectatorsColor, spectatorsXAlign)				
			end
		end
	end
end

function lightesp.thinkDestroy()
	local destroy = true
	for k, v in ipairs(lightesp.destroyKeys) do
		if not input.IsKeyDown(v) then
			destroy = false
			break
		end
	end
	if destroy == true then
		hook.Remove('Think', 'lightespDestroy')
		hook.Remove('DrawOverlay', 'lightesp')
		lightesp = nil
	end
end

hook.Add('DrawOverlay', 'lightesp', lightesp.drawoverlay)

hook.Add('Think', 'lightespDestroy', lightesp.thinkDestroy)