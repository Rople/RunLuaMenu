lightimer = {}

lightimer.configsTimer = {}

lightimer.configsTimerJSON = {}

lightimer.config = {}

lightimer.functions = { RunConsoleCommand = { 'string', 'vararg' }, netSendToServer = { }, netStart = { 'string', 'boolean' }, netWriteAngle = { 'Angle' }, netWriteBit = { 'boolean' }, netWriteBool = { 'boolean' }, netWriteColor = { 'table' }, netWriteData = { 'string', 'number' }, netWriteDouble = { 'number' }, netWriteFloat = { 'number' }, netWriteInt = { 'number', 'number' }, netWriteNormal = { 'Vector' }, netWriteString = { 'string' }, netWriteTable = { 'table' }, netWriteUInt = { 'number', 'number' }, netWriteVector = { 'Vector' } }

lightimer.configsTimerExecuteIndex = {}
lightimer.configsTimerExecuteOnSpawn = {}
lightimer.configsTimerExecuteLastTime = {}

lightimer.configRefreshDelay = 1
lightimer.configRefreshExecuteLastTime = nil

lightimer.configFolder = ''
lightimer.configFile = ''

lightimer.destroyKeys = { KEY_EQUAL, KEY_BACKSPACE }

lightimer.clientLuaValid = false

lightimer.resetWhenClientLuaNotValid = true

lightimer.startDelay = 1
lightimer.startExecuteLastTime = nil
lightimer.started = false

if lightimer.configFolder == '' or lightimer.configFile == '' then
	return
end

function lightimer.getConfig()
	if not gaceioIsDir( lightimer.configFolder) then
		gaceioCreateDir( lightimer.configFolder)
	end
	if not gaceioExists( lightimer.configFolder .. '/' .. lightimer.configFile) then
		gaceioWrite( lightimer.configFolder .. '/' .. lightimer.configFile, '{ }')
	end
	local newConfig = {}
	local content = gaceioRead( lightimer.configFolder .. '/' .. lightimer.configFile)
	local t = util.JSONToTable(content)
	if t ~= nil then
		local err = false
		for k, v in pairs(t) do
			if type(k) ~= 'string' or type(v) ~= 'boolean' or v == false then
				err = true
				break
			end
		end
		if not err then
			newConfig = t
		end
	end
	return newConfig
end

function lightimer.getConfigsTimer()
	local newConfigsTimer = {}
	local newConfigsTimerJSON = {}
	local files, _ = gaceioList( lightimer.configFolder)
	for _, name in pairs(files) do
		if name ~= lightimer.configFile then
			local content = gaceioRead( lightimer.configFolder .. '/' .. name)
			local t = util.JSONToTable(content)
			if t ~= nil then
				local count = table.Count(t)
				if count == 2 then
					if t.executeIfDead ~= nil and type(t.executeIfDead) == 'boolean' then
						if t.json ~= nil and type(t.json) == 'table' then
							if table.IsSequential(t.json) and #t.json > 0 then
								local err = false
								for k, v in pairs(t.json) do
									if type(v) ~= 'table' then
										err = true
										break
									end
									local count2 = table.Count(v)
									if count2 ~= 3 then
										err = true
										break
									end
									if v.functions == nil or type(v.functions) ~= 'table' then
										err = true
										break
									end
									if not table.IsSequential(v.functions) then
										err = true
										break
									end
									if v.arguments == nil or type(v.arguments) ~= 'table' then
										err = true
										break
									end
									if not table.IsSequential(v.arguments) then
										err = true
										break
									end
									if v.delay == nil or type(v.delay) ~= 'number' then
										err = true
										break
									end
									local count3 = #v.functions
									local count4 = #v.arguments
									if  count3 < 1 or count3 ~= count4 then
										err = true
										break
									end
									local err2 = false
									for k2, v2 in pairs(v.functions) do
										if type(v2) ~= 'string' then
											err2 = true
											break
										end
										if lightimer.functions[v2] == nil then
											err2 = true
											break
										end
									end
									if err2 then
										err = true
										break
									end
									local err3 = false
									for k2, v2 in pairs(v.arguments) do
										if type(v2) ~= 'table' then
											err3 = true
											break
										end
										if not table.IsSequential(v2) then
											err3 = true
											break
										end
										local fn = v.functions[k2]
										local args = lightimer.functions[fn]
										local count5 = #v2
										local count6 = #args
										local varargBypass = false
										if args[count6] == 'vararg' then
											varargBypass = true
										end
										if count5 < count6 then
											err3 = true
											break
										end
										if varargBypass == false and count5 > count6 then
											err3 = true
											break
										end
										local err4 = false
										for k3, v3 in pairs(v2) do
											if k3 == count6 and varargBypass == true then
												break
											end
											if args[k3] ~= 'any' and type(v3) ~= args[k3] then
												err4 = true
												break
											end
										end
										if err4 then
											err3 = true
											break
										end
									end
									if err3 then
										err = true
										break
									end
								end
								if not err then
									local nameWithoutJSON = string.sub(name, 1, string.len(name) - string.len('.json'))
									newConfigsTimer[nameWithoutJSON] = t
									newConfigsTimerJSON[nameWithoutJSON] = content
								end
							end
						end
					end
				end
			end
		end
	end
	return newConfigsTimer, newConfigsTimerJSON
end

function lightimer.setConfig(newConfig)
	lightimer.config = newConfig
	for k, v in pairs( lightimer.configsTimerExecuteIndex) do
		if lightimer.config[k] == nil then
			lightimer.configsTimerExecuteIndex[k] = nil
		end
	end
	for k, v in pairs( lightimer.configsTimerExecuteOnSpawn) do
		if lightimer.config[k] == nil then
			lightimer.configsTimerExecuteOnSpawn[k] = nil
		end
	end
	for k, v in pairs( lightimer.configsTimerExecuteLastTime) do
		if lightimer.config[k] == nil then
			lightimer.configsTimerExecuteLastTime[k] = nil
		end
	end
end

function lightimer.setConfigsTimer(newConfigsTimer, newConfigsTimerJSON)
	lightimer.configsTimer = newConfigsTimer
	local changedContent = {}
	for k, v in pairs(newConfigsTimerJSON) do
		if lightimer.configsTimerJSON[k] ~= nil and lightimer.configsTimerJSON[k] ~= v then
			changedContent[k] = true
		end
	end
	lightimer.configsTimerJSON = newConfigsTimerJSON
	for k, v in pairs( lightimer.configsTimerExecuteIndex) do
		if lightimer.configsTimerJSON[k] == nil or changedContent[k] == true then
			lightimer.configsTimerExecuteIndex[k] = nil
		end
	end
	for k, v in pairs( lightimer.configsTimerExecuteOnSpawn) do
		if lightimer.configsTimerJSON[k] == nil or changedContent[k] == true then
			lightimer.configsTimerExecuteOnSpawn[k] = nil
		end
	end
	for k, v in pairs( lightimer.configsTimerExecuteLastTime) do
		if lightimer.configsTimerJSON[k] == nil or changedContent[k] == true then
			lightimer.configsTimerExecuteLastTime[k] = nil
		end
	end
end

function lightimer.think()
	if  gaceioList ~= nil and gaceioRead ~= nil and gaceioWrite ~= nil and gaceioDelete ~= nil and gaceioIsDir ~= nil and gaceioExists ~= nil and gaceioCreateDir ~= nil and gaceioTime ~= nil and gaceioSize ~= nil and gaceioCRC ~= nil and netSendToServer ~= nil and netStart ~= nil and netWriteAngle ~= nil and netWriteBit ~= nil and netWriteBool ~= nil and netWriteColor ~= nil and netWriteData ~= nil and netWriteDouble ~= nil and netWriteFloat ~= nil and netWriteInt ~= nil and netWriteNormal ~= nil and netWriteString ~= nil and netWriteTable ~= nil and netWriteUInt ~= nil and netWriteVector ~= nil and IsClientLuaValid ~= nil and LocalPlayerIsValid ~= nil and LocalPlayerAlive ~= nil then
		if lightimer.configRefreshExecuteLastTime == nil or CurTime() - lightimer.configRefreshExecuteLastTime >= lightimer.configRefreshDelay then
			local config = lightimer.getConfig()
			local configsTimer, configsTimerJSON = lightimer.getConfigsTimer()
			lightimer.setConfig(config)
			lightimer.setConfigsTimer(configsTimer, configsTimerJSON)
			lightimer.configRefreshExecuteLastTime = CurTime()
		end
		if not IsClientLuaValid() then
			if lightimer.clientLuaValid == true then
				lightimer.clientLuaValid = false
				lightimer.startExecuteLastTime = nil
				lightimer.started = false
				if lightimer.resetWhenClientLuaNotValid == true then
					lightimer.configsTimerExecuteIndex = {}
					lightimer.configsTimerExecuteOnSpawn = {}
					lightimer.configsTimerExecuteLastTime = {}
				end
			end
			return
		else
			if lightimer.clientLuaValid == false then
				lightimer.clientLuaValid = true
			end
		end
		if not LocalPlayerIsValid() then
			return
		end
		if lightimer.started == false then
			if lightimer.startExecuteLastTime == nil then
				lightimer.startExecuteLastTime = CurTime()
			end
			if CurTime() - lightimer.startExecuteLastTime >= lightimer.startDelay then
				lightimer.started = true
				lightimer.startExecuteLastTime = nil
			else
				return
			end
		end
		local isalive = LocalPlayerAlive()
		for k, v in pairs( lightimer.config) do
			local configTimer = lightimer.configsTimer[k]
			if configTimer ~= nil then
				lightimer.configsTimerExecuteIndex[k] = lightimer.configsTimerExecuteIndex[k] or 0
				lightimer.configsTimerExecuteOnSpawn[k] = lightimer.configsTimerExecuteOnSpawn[k] or false
				if lightimer.configsTimerExecuteOnSpawn[k] then
					if isalive then
						lightimer.executeFunctions(configTimer.json[ lightimer.configsTimerExecuteIndex[k] ].functions, configTimer.json[ lightimer.configsTimerExecuteIndex[k] ].arguments)
						lightimer.configsTimerExecuteLastTime[k] = CurTime()
						lightimer.configsTimerExecuteOnSpawn[k] = false
					end
				else
					if lightimer.configsTimerExecuteLastTime[k] == nil or CurTime() - lightimer.configsTimerExecuteLastTime[k] >= configTimer.json[ lightimer.configsTimerExecuteIndex[k] ].delay then
						if #configTimer.json == lightimer.configsTimerExecuteIndex[k] then
							lightimer.configsTimerExecuteIndex[k] = 0
						end
						lightimer.configsTimerExecuteIndex[k] = lightimer.configsTimerExecuteIndex[k] + 1
						if configTimer.executeIfDead or isalive then
							lightimer.executeFunctions(configTimer.json[ lightimer.configsTimerExecuteIndex[k] ].functions, configTimer.json[ lightimer.configsTimerExecuteIndex[k] ].arguments)
							lightimer.configsTimerExecuteLastTime[k] = CurTime()
						else
							lightimer.configsTimerExecuteOnSpawn[k] = true
						end
					end
				end
			end
		end
	end
end

function lightimer.executeFunctions(functions, arguments)
	for k, v in ipairs(functions) do
		local args = arguments[k]
		if v == 'RunConsoleCommand' then
			local cmd = args[1]
			local cmdArgs = table.Copy( args)
			table.remove( cmdArgs, 1 )
			RunConsoleCommand( cmd, unpack(args))		
		elseif v == 'netSendToServer' then
			netSendToServer( unpack(args))
		elseif v == 'netStart' then
			netStart( unpack(args))
		elseif v == 'netWriteAngle' then
			netWriteAngle( unpack(args))
		elseif v == 'netWriteBit' then
			netWriteBit( unpack(args))
		elseif v == 'netWriteBool' then
			netWriteBool( unpack(args))
		elseif v == 'netWriteColor' then
			netWriteColor( unpack(args))
		elseif v == 'netWriteData' then
			netWriteData( unpack(args))
		elseif v == 'netWriteDouble' then
			netWriteDouble( unpack(args))
		elseif v == 'netWriteEntity' then
			netWriteEntity( unpack(args))
		elseif v == 'netWriteFloat' then
			netWriteFloat( unpack(args))
		elseif v == 'netWriteInt' then
			netWriteInt( unpack(args))
		elseif v == 'netWriteMatrix' then
			netWriteMatrix( unpack(args))
		elseif v == 'netWriteNormal' then
			netWriteNormal( unpack(args))
		elseif v == 'netWriteString' then
			netWriteString( unpack(args))
		elseif v == 'netWriteTable' then
			netWriteTable( unpack(args))
		elseif v == 'netWriteType' then
			netWriteType( unpack(args))
		elseif v == 'netWriteUInt' then
			netWriteUInt( unpack(args))
		elseif v == 'netWriteVector' then
			netWriteVector( unpack(args))
		end
	end
end

function lightimer.thinkDestroy()
	local destroy = true
	for k, v in ipairs(lightimer.destroyKeys) do
		if not input.IsKeyDown(v) then
			destroy = false
			break
		end
	end
	if destroy == true then
		hook.Remove('Think', 'lightimerDestroy')
		hook.Remove('Think', 'lightimer')
		lightimer = nil
	end
end

hook.Add('Think', 'lightimer', lightimer.think)

hook.Add('Think', 'lightimerDestroy', lightimer.thinkDestroy)