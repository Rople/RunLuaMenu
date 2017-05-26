# RunLuaMenu
Run lua on menu state on Garry's Mod
Lua:
- lightimer: Run command or send net messages at specific interval. The texthat of pointshop was not really customizable, so I did it :)
# Installation
1. Download an injector
2. Download the lastest release (https://github.com/Lightoner/RunLuaMenu/releases/latest)
3. Create a folder (not in Garry's Mod directory, it can be detected !)
4. Copy the injector and the folders of the release in the created folder
5. Go in the lua folder and open lightimer.lua and edit these two lines with your lightimer directory and config file name, then save:
   lightimer.configFolder = ''
   lightimer.configFile = ''
   example:
   lightimer.configFolder = 'D:/injector/lua/lightimer'
   lightimer.configFile = 'config.json'
6. Launch Garry's Mod
7. Launch the injector
8. Select the process of Garry's Mod
9. Add RunLuaMenu.dll in the injector
10. Inject
11. If you are in the main menu, the RunLuaMenu panel should appear
    Put the path of your lua file in the text box and then click on the button to run your lua file on the menu state
12. Run your lua file
    example if I want to run lightimer.lua, I put:
    D:/injector/lua/lightimer.lua
13. If you have run lightimer.lua and you have activated the texthat config, join a server with the pointshop addon and equip your texthat
14. If you have done everything correctly, your texthat should change depending on your texthat config. If not, then you are a retard
# Notes
1. If your Garry's Mod crash with an error like unpooled net message, you are joining a server with a config that use net message and the net message can't be sent to the server. Example: if you use a texthat config (because it sends net messages), then be sure the server you are joining have the pointshop addon, if not you will have this error. If you want to disable the texthat config remove it in the your config file. Example: { "rtd": true, "rainbow_vac": true }, will be { "rtd": true }. When you are modifying the config file or any active config. lightimer.lua will automatically refresh
2. The RunLuaMenu panel appear only when you are on the main menu. It doesn't appear in game to not be detected by screenshot
3. You can hide the panel in the main menu by pressing at the same time the close bracket key: ')' and the equal key: '='. For me they are between the zero key and the backspace key
4. You can eject the dll by pressing at the same time the equal key: '=' and the backspace key. Note that this will also uninitialize the lightimer.lua script because they have the same keys to destroy. Then you will have a clean Garry's Mod. And you can delete the folder if you need.
5. It hard to detect it, but it can be detected. Use at your own risk
