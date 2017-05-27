#include <Windows.h>
#include "Bootil/Bootil.h"
#include "GLua.h"

ILuaShared* LuaShared = NULL;
ILuaInterface* MenuLua = NULL;

//gaceio start
#ifdef __linux__
#include <dirent.h>
#endif

void TraverseFolder(const Bootil::BString& folder, Bootil::String::List* files, Bootil::String::List* folders) {
	// Apparently Bootil returns FULL (absolute) paths when File::Find is used on Linux. We don't want that..

#ifdef __linux__
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(folder.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			// Ignore symlinks etc
			if (ent->d_type == DT_REG) {
				files->push_back(ent->d_name);
			}
			else if (ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
				folders->push_back(ent->d_name);
			}
		}
		closedir(dir);
	}
#endif

#ifdef _WIN32
	// Bootil does a good job when it's Windows, so we'll just use Bootil code here
	Bootil::File::Find(files, folders, folder + "/*", false);
#endif

}

int LuaFunc_ListDir(lua_State* state)
{
	const Bootil::BString& folder = MenuLua->CheckString(1);

	if (!Bootil::File::Exists(folder)) {
		MenuLua->PushBool(false);
		MenuLua->PushString("Folder does not exist");
		return 2;
	}

	if (!Bootil::File::IsFolder(folder)) {
		MenuLua->PushBool(false);
		MenuLua->PushString("Cannot list a non-folder");
		return 2;
	}


	Bootil::String::List files;
	Bootil::String::List folders;

	TraverseFolder(folder, &files, &folders);

	// Files
	MenuLua->CreateTable();
	int i = 1;

	BOOTIL_FOREACH(f, files, Bootil::String::List) {
		MenuLua->PushNumber(i);
		MenuLua->PushString((*f).c_str());
		MenuLua->SetTable(-3);

		i++;
	}

	// Folders
	MenuLua->CreateTable();
	i = 1;

	BOOTIL_FOREACH(f, folders, Bootil::String::List) {
		MenuLua->PushNumber(i);
		MenuLua->PushString((*f).c_str());
		MenuLua->SetTable(-3);

		i++;
	}

	return 2;
}

int LuaFunc_ReadFile(lua_State* state)
{
	Bootil::BString out;
	if (!Bootil::File::Read(MenuLua->CheckString(1), out)) {
		MenuLua->PushBool(false);
		MenuLua->PushString(Bootil::Platform::LastError().c_str());
		return 2;
	}

	MenuLua->PushString(out.c_str(), out.length());
	return 1;
}

int LuaFunc_Delete(lua_State* state)
{
	const Bootil::BString& targ = MenuLua->CheckString(1);

	bool success;
	if (Bootil::File::IsFolder(targ)) {
		success = Bootil::File::RemoveFolder(targ); // TODO should we recursively remove (2nd param)?
	}
	else {
		success = Bootil::File::RemoveFile(targ);
	}

	if (!success) {
		MenuLua->PushBool(false);
		MenuLua->PushString(Bootil::Platform::LastError().c_str());
		return 2;
	}

	return 0;
}

int LuaFunc_WriteToFile(lua_State* state)
{
	const char* path = MenuLua->CheckString(1);

	MenuLua->CheckString(2);

	// If data string contains null bytes, the string is prematurely truncated unless we get the size explicitly
	size_t len;
	const char* data = MenuLua->GetString(2, &len);

	const Bootil::BString &strOut = std::string(data, len);

	if (!Bootil::File::Write(path, strOut)) {
		MenuLua->PushBool(false);
		MenuLua->PushString(Bootil::Platform::LastError().c_str());
		return 2;
	}

	return 0;
}

int LuaFunc_IsFolder(lua_State* state)
{
	MenuLua->PushBool(Bootil::File::IsFolder(MenuLua->CheckString(1)));
	return 1;
}

int LuaFunc_Exists(lua_State* state)
{
	// Bootil::File::Exists actually returns a boolean
	MenuLua->PushBool(Bootil::File::Exists(MenuLua->CheckString(1)));
	return 1;
}

int LuaFunc_CreateFolder(lua_State* state)
{
	bool success = Bootil::File::CreateFolder(MenuLua->CheckString(1));

	if (!success) {
		MenuLua->PushBool(false);
		MenuLua->PushString(Bootil::Platform::LastError().c_str());
		return 2;
	}

	return 0;
}

int FileLastModified(const Bootil::BString& strFileName) {
	struct stat fileStat;
	int err = stat(strFileName.c_str(), &fileStat);
	if (err != 0) { return 0; }
	return fileStat.st_mtime;
}


int LuaFunc_Time(lua_State* state)
{
	MenuLua->PushNumber(FileLastModified(MenuLua->CheckString(1)));
	return 1;
}

int LuaFunc_Size(lua_State* state)
{
	MenuLua->PushNumber(Bootil::File::Size(MenuLua->CheckString(1)));
	return 1;
}

int LuaFunc_CRC(lua_State* state)
{
	MenuLua->PushNumber(Bootil::File::CRC(MenuLua->CheckString(1)));
	return 1;
}

//gaceio end

int RunLuaMenu(lua_State* state)
{
	std::string filename = "RunString";
	std::string path;
	std::string torun;
	bool run = true;
	bool showerrors = true;
	if (MenuLua->IsType(1, Type::STRING))
	{
		unsigned int i;
		const char* str = MenuLua->GetString(1, &i);
		std::string targetFile = std::string(str, i);

		Bootil::BString out;
		Bootil::File::Read(targetFile, out);

		torun = std::string(out.c_str(), out.length());
	}
	if (MenuLua->IsType(2, Type::STRING))
	{
		unsigned int i;
		const char* str = MenuLua->GetString(2, &i);
		filename = std::string(str, i);
	}
	if (MenuLua->IsType(3, Type::STRING))
	{
		unsigned int i;
		const char* str = MenuLua->GetString(3, &i);
		path = std::string(str, i);
	}
	if (MenuLua->IsType(4, Type::BOOL))
	{
		run = MenuLua->GetBool(4);
	}
	if (MenuLua->IsType(5, Type::BOOL))
	{
		showerrors = MenuLua->GetBool(5);
	}
	MenuLua->RunString(filename.c_str(), path.c_str(), torun.c_str(), run, showerrors);
	return 0;
}

int IsClientLuaValid(lua_State* state)
{
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	if (ClientLua != NULL)
	{
		MenuLua->PushBool(true);
	}
	else
	{
		MenuLua->PushBool(false);
	}
	return 1;
}

int LocalPlayerIsValid(lua_State* state)
{
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "IsValid");
	ClientLua->GetField(-2, "LocalPlayer");
	ClientLua->Remove(-3);
	ClientLua->Call(0, 1);
	ClientLua->Call(1, 1);
	bool b = ClientLua->GetBool(-1);
	ClientLua->Pop();
	MenuLua->PushBool(b);
	return 1;
}

int LocalPlayerAlive(lua_State* state)
{
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "debug");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "getregistry");
	ClientLua->Remove(-2);
	ClientLua->Call(0, 1);
	ClientLua->GetField(-1, "Player");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "Alive");
	ClientLua->Remove(-2);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "LocalPlayer");
	ClientLua->Remove(-2);
	ClientLua->Call(0, 1);
	ClientLua->Call(1, 1);
	bool b = ClientLua->GetBool(-1);
	ClientLua->Pop();
	MenuLua->PushBool(b);
	return 1;
}

int netStart(lua_State* state)
{
	std::string messageName;
	bool unreliable = false;
	if (MenuLua->IsType(1, Type::STRING))
	{
		unsigned int i;
		const char* str = MenuLua->GetString(1, &i);
		messageName = std::string(str, i);
	}
	if (MenuLua->IsType(2, Type::BOOL))
	{
		unreliable = MenuLua->GetBool(2);
	}
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "net");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "Start");
	ClientLua->Remove(-2);
	ClientLua->PushString(messageName.c_str(), messageName.length());
	ClientLua->PushBool(unreliable);
	ClientLua->Call(2, 1);
	bool b = ClientLua->GetBool(-1);
	ClientLua->Pop();
	MenuLua->PushBool(b);
	return 1;
}

int netSendToServer(lua_State* state)
{
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "net");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "SendToServer");
	ClientLua->Remove(-2);
	ClientLua->Call(0, 0);
	return 0;
}

void getAngle(double pitch, double yaw, double roll)
{
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "Angle");
	ClientLua->Remove(-2);
	ClientLua->PushNumber(pitch);
	ClientLua->PushNumber(yaw);
	ClientLua->PushNumber(roll);
	ClientLua->Call(3, 1);
}

int netWriteAngle(lua_State* state)
{
	double pitch = 0;
	double yaw = 0;
	double roll = 0;
	if (MenuLua->IsType(1, Type::ANGLE))
	{
		MenuLua->GetField(1, "pitch");
		pitch = MenuLua->GetNumber(-1);
		MenuLua->Pop();
		MenuLua->GetField(1, "yaw");
		yaw = MenuLua->GetNumber(-1);
		MenuLua->Pop();
		MenuLua->GetField(1, "roll");
		roll = MenuLua->GetNumber(-1);
		MenuLua->Pop();
	}
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "net");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "WriteAngle");
	ClientLua->Remove(-2);
	getAngle(pitch, yaw, roll);
	ClientLua->Call(1, 0);
	return 0;
}

int netWriteBit(lua_State* state)
{
	bool b = false;
	if (MenuLua->IsType(1, Type::BOOL))
	{
		b = MenuLua->GetBool(1);
	}
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "net");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "WriteBit");
	ClientLua->Remove(-2);
	ClientLua->PushBool(b);
	ClientLua->Call(1, 0);
	return 0;
}

int netWriteBool(lua_State* state)
{
	bool b = false;
	if (MenuLua->IsType(1, Type::BOOL))
	{
		b = MenuLua->GetBool(1);
	}
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "net");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "WriteBool");
	ClientLua->Remove(-2);
	ClientLua->PushBool(b);
	ClientLua->Call(1, 0);
	return 0;
}

void getColor(double r, double g, double b, double a)
{
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "Color");
	ClientLua->Remove(-2);
	ClientLua->PushNumber(r);
	ClientLua->PushNumber(g);
	ClientLua->PushNumber(b);
	ClientLua->PushNumber(a);
	ClientLua->Call(4, 1);
}

int netWriteColor(lua_State* state)
{

	double r = 0;
	double g = 0;
	double b = 0;
	double a = 255;
	if (MenuLua->IsType(1, Type::TABLE))
	{
		MenuLua->GetField(1, "r");
		r = MenuLua->GetNumber(-1);
		MenuLua->Pop();
		MenuLua->GetField(1, "g");
		g = MenuLua->GetNumber(-1);
		MenuLua->Pop();
		MenuLua->GetField(1, "b");
		b = MenuLua->GetNumber(-1);
		MenuLua->Pop();
		MenuLua->GetField(1, "a");
		a = MenuLua->GetNumber(-1);
		MenuLua->Pop();
	}
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "net");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "WriteColor");
	ClientLua->Remove(-2);
	getColor(r, g, b, a);
	ClientLua->Call(1, 0);
	return 0;
}

int netWriteData(lua_State* state)
{
	std::string binaryData;
	double length = 0;
	if (MenuLua->IsType(1, Type::STRING))
	{
		unsigned int i;
		const char* str = MenuLua->GetString(1, &i);
		binaryData = std::string(str, i);
	}
	if (MenuLua->IsType(2, Type::NUMBER))
	{
		length = MenuLua->GetNumber(2);
	}
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "net");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "WriteData");
	ClientLua->Remove(-2);
	ClientLua->PushString(binaryData.c_str(), binaryData.length());
	ClientLua->PushNumber(length);
	ClientLua->Call(2, 0);
	return 0;
}

int netWriteDouble(lua_State* state)
{
	double d = 0;
	if (MenuLua->IsType(1, Type::NUMBER))
	{
		d = MenuLua->GetNumber(1);
	}
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "net");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "WriteDouble");
	ClientLua->Remove(-2);
	ClientLua->PushNumber(d);
	ClientLua->Call(1, 0);
	return 0;
}

int netWriteFloat(lua_State* state)
{
	double d = 0;
	if (MenuLua->IsType(1, Type::NUMBER))
	{
		d = MenuLua->GetNumber(1);
	}
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "net");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "WriteFloat");
	ClientLua->Remove(-2);
	ClientLua->PushNumber(d);
	ClientLua->Call(1, 0);
	return 0;
}

int netWriteInt(lua_State* state)
{
	double d = 0;
	double bitCount = 0;
	if (MenuLua->IsType(1, Type::NUMBER))
	{
		d = MenuLua->GetNumber(1);
	}
	if (MenuLua->IsType(2, Type::NUMBER))
	{
		bitCount = MenuLua->GetNumber(2);
	}
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "net");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "WriteInt");
	ClientLua->Remove(-2);
	ClientLua->PushNumber(d);
	ClientLua->PushNumber(bitCount);
	ClientLua->Call(2, 0);
	return 0;
}

void getVector(double x, double y, double z)
{
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "Vector");
	ClientLua->Remove(-2);
	ClientLua->PushNumber(x);
	ClientLua->PushNumber(y);
	ClientLua->PushNumber(z);
	ClientLua->Call(3, 1);
}

int netWriteNormal(lua_State* state)
{
	double x = 0;
	double y = 0;
	double z = 0;
	if (MenuLua->IsType(1, Type::VECTOR))
	{
		MenuLua->GetField(1, "x");
		x = MenuLua->GetNumber(-1);
		MenuLua->Pop();
		MenuLua->GetField(1, "y");
		y = MenuLua->GetNumber(-1);
		MenuLua->Pop();
		MenuLua->GetField(1, "z");
		z = MenuLua->GetNumber(-1);
		MenuLua->Pop();
	}
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "net");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "WriteNormal");
	ClientLua->Remove(-2);
	getVector(x, y, z);
	ClientLua->Call(1, 0);
	return 0;
}

int netWriteString(lua_State* state)
{
	std::string s;
	if (MenuLua->IsType(1, Type::STRING))
	{
		unsigned int i;
		const char* str = MenuLua->GetString(1, &i);
		s = std::string(str, i);
	}
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "net");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "WriteString");
	ClientLua->Remove(-2);
	ClientLua->PushString(s.c_str(), s.length());
	ClientLua->Call(1, 0);
	return 0;
}

std::string utilTableToJSON(int iStackPos)
{
	MenuLua->PushSpecial(SPECIAL_GLOB);
	MenuLua->GetField(-1, "util");
	MenuLua->Remove(-2);
	MenuLua->GetField(-1, "TableToJSON");
	MenuLua->Remove(-2);
	MenuLua->Push(iStackPos);
	MenuLua->PushBool(false);
	MenuLua->Call(2, 1);
	unsigned int i;
	const char* str = MenuLua->GetString(-1, &i);
	std::string s = std::string(str, i);
	MenuLua->Pop();
	return s;
}

void utilJSONToTable(std::string json)
{
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "util");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "JSONToTable");
	ClientLua->Remove(-2);
	ClientLua->PushString(json.c_str(), json.length());
	ClientLua->Call(1, 1);
}

int netWriteTable(lua_State* state)
{
	std::string json;
	if (MenuLua->IsType(1, Type::TABLE))
	{
		json = utilTableToJSON(1);
	}
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "net");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "WriteTable");
	ClientLua->Remove(-2);
	utilJSONToTable(json);
	ClientLua->Call(1, 0);
	return 0;
}

int netWriteUInt(lua_State* state)
{
	double d = 0;
	double numberOfBits = 0;
	if (MenuLua->IsType(1, Type::NUMBER))
	{
		d = MenuLua->GetNumber(1);
	}
	if (MenuLua->IsType(2, Type::NUMBER))
	{
		numberOfBits = MenuLua->GetNumber(2);
	}
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "net");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "WriteUInt");
	ClientLua->Remove(-2);
	ClientLua->PushNumber(d);
	ClientLua->PushNumber(numberOfBits);
	ClientLua->Call(2, 0);
	return 0;
}

int netWriteVector(lua_State* state)
{
	double x = 0;
	double y = 0;
	double z = 0;
	if (MenuLua->IsType(1, Type::VECTOR))
	{
		MenuLua->GetField(1, "x");
		x = MenuLua->GetNumber(-1);
		MenuLua->Pop();
		MenuLua->GetField(1, "y");
		y = MenuLua->GetNumber(-1);
		MenuLua->Pop();
		MenuLua->GetField(1, "z");
		z = MenuLua->GetNumber(-1);
		MenuLua->Pop();
	}
	ILuaInterface* ClientLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
	ClientLua->PushSpecial(SPECIAL_GLOB);
	ClientLua->GetField(-1, "net");
	ClientLua->Remove(-2);
	ClientLua->GetField(-1, "WriteVector");
	ClientLua->Remove(-2);
	getVector(x, y, z);
	ClientLua->Call(1, 0);
	return 0;
}

HMODULE module = NULL;

int DllUninit()
{
	FreeLibraryAndExitThread(module, 0);
}

int RunLuaMenuDestroy(lua_State* state)
{
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DllUninit, NULL, NULL, NULL);
	return 0;
}

void hookAddMenu(std::string name, std::string identifier, CLuaFunction function)
{
	MenuLua->PushSpecial(SPECIAL_GLOB);
	MenuLua->GetField(-1, "hook");
	MenuLua->GetField(-1, "Add");
	MenuLua->PushString(name.c_str(), name.length());
	MenuLua->PushString(identifier.c_str(), identifier.length());
	MenuLua->PushCFunction(function);
	MenuLua->Call(3, 0);
	MenuLua->Pop();
	MenuLua->Pop();
}

void hookRemoveMenu(std::string name, std::string identifier)
{
	MenuLua->PushSpecial(SPECIAL_GLOB);
	MenuLua->GetField(-1, "hook");
	MenuLua->GetField(-1, "Remove");
	MenuLua->PushString(name.c_str(), name.length());
	MenuLua->PushString(identifier.c_str(), identifier.length());
	MenuLua->Call(2, 0);
	MenuLua->Pop();
	MenuLua->Pop();
}

int RunLuaMenuInit(lua_State* state)
{
	hookRemoveMenu("Think", "RunLuaMenuInit");
	MenuLua->PushSpecial(SPECIAL_GLOB);
	//gaceio start
	MenuLua->PushCFunction(LuaFunc_ListDir);
	MenuLua->SetField(-2, "gaceioList");
	MenuLua->PushCFunction(LuaFunc_ReadFile);
	MenuLua->SetField(-2, "gaceioRead");
	MenuLua->PushCFunction(LuaFunc_WriteToFile);
	MenuLua->SetField(-2, "gaceioWrite");
	MenuLua->PushCFunction(LuaFunc_Delete);
	MenuLua->SetField(-2, "gaceioDelete");
	MenuLua->PushCFunction(LuaFunc_IsFolder);
	MenuLua->SetField(-2, "gaceioIsDir");
	MenuLua->PushCFunction(LuaFunc_Exists);
	MenuLua->SetField(-2, "gaceioExists");
	MenuLua->PushCFunction(LuaFunc_CreateFolder);
	MenuLua->SetField(-2, "gaceioCreateDir");
	MenuLua->PushCFunction(LuaFunc_Time);
	MenuLua->SetField(-2, "gaceioTime");
	MenuLua->PushCFunction(LuaFunc_Size);
	MenuLua->SetField(-2, "gaceioSize");
	MenuLua->PushCFunction(LuaFunc_CRC);
	MenuLua->SetField(-2, "gaceioCRC");
	//gaceio end
	MenuLua->PushCFunction(RunLuaMenu);
	MenuLua->SetField(-2, "RunLuaMenu");
	MenuLua->PushCFunction(RunLuaMenuDestroy);
	MenuLua->SetField(-2, "RunLuaMenuDestroy");
	MenuLua->PushCFunction(IsClientLuaValid);
	MenuLua->SetField(-2, "IsClientLuaValid");
	MenuLua->PushCFunction(LocalPlayerIsValid);
	MenuLua->SetField(-2, "LocalPlayerIsValid");
	MenuLua->PushCFunction(LocalPlayerAlive);
	MenuLua->SetField(-2, "LocalPlayerAlive");
	MenuLua->PushCFunction(netStart);
	MenuLua->SetField(-2, "netStart");
	MenuLua->PushCFunction(netSendToServer);
	MenuLua->SetField(-2, "netSendToServer");
	MenuLua->PushCFunction(netWriteAngle);
	MenuLua->SetField(-2, "netWriteAngle");
	MenuLua->PushCFunction(netWriteBit);
	MenuLua->SetField(-2, "netWriteBit");
	MenuLua->PushCFunction(netWriteBool);
	MenuLua->SetField(-2, "netWriteBool");
	MenuLua->PushCFunction(netWriteColor);
	MenuLua->SetField(-2, "netWriteColor");
	MenuLua->PushCFunction(netWriteData);
	MenuLua->SetField(-2, "netWriteData");
	MenuLua->PushCFunction(netWriteDouble);
	MenuLua->SetField(-2, "netWriteDouble");
	MenuLua->PushCFunction(netWriteFloat);
	MenuLua->SetField(-2, "netWriteFloat");
	MenuLua->PushCFunction(netWriteInt);
	MenuLua->SetField(-2, "netWriteInt");
	MenuLua->PushCFunction(netWriteNormal);
	MenuLua->SetField(-2, "netWriteNormal");
	MenuLua->PushCFunction(netWriteString);
	MenuLua->SetField(-2, "netWriteString");
	MenuLua->PushCFunction(netWriteTable);
	MenuLua->SetField(-2, "netWriteTable");
	MenuLua->PushCFunction(netWriteUInt);
	MenuLua->SetField(-2, "netWriteUInt");
	MenuLua->PushCFunction(netWriteVector);
	MenuLua->SetField(-2, "netWriteVector");
	MenuLua->Pop();
	MenuLua->RunString("RunString", "", "RunLuaMenuDestroyKeys = { KEY_EQUAL, KEY_BACKSPACE } RunLuaMenuToggleKeys = { KEY_EQUAL, KEY_LBRACKET } RunLuaMenuToggle = true RunLuaMenuToggleDelay = 0.5 RunLuaMenuToggleExecuteLastTime = nil function thinkRunLuaMenuDestroy() local destroy = true for k, v in ipairs(RunLuaMenuDestroyKeys) do if not input.IsKeyDown(v) then destroy = false break end end if destroy == true then if RunLuaMenuDestroy ~= nil then hook.Remove('Think', 'RunLuaMenuDestroy') hook.Remove('Think', 'RunLuaMenuHide') RunLuaMenuDestroyKeys = nil RunLuaMenuToggleKeys = nil RunLuaMenuToggle = nil RunLuaMenuToggleDelay = nil RunLuaMenuToggleExecuteLastTime = nil menuRunLuaMenuPosX = nil menuRunLuaMenuPosY = nil if textRunLuaMenu ~= nil and buttonRunLuaMenu ~= nil and menuRunLuaMenu ~= nil then textRunLuaMenu:Remove() textRunLuaMenu = nil buttonRunLuaMenu:Remove() buttonRunLuaMenu = nil menuRunLuaMenu:Remove() menuRunLuaMenu = nil end --[[ gaceio start --]] gaceioList = nil gaceioRead = nil gaceioWrite = nil gaceioDelete = nil gaceioIsDir = nil gaceioExists = nil gaceioCreateDir = nil gaceioTime = nil gaceioSize = nil gaceioCRC = nil --[[ gaceio end --]] RunLuaMenu = nil IsClientLuaValid = nil LocalPlayerIsValid = nil LocalPlayerAlive = nil netStart = nil netSendToServer = nil netWriteAngle = nil netWriteBit = nil netWriteBool = nil netWriteColor = nil netWriteData = nil netWriteDouble = nil netWriteFloat = nil netWriteInt = nil netWriteNormal = nil netWriteString = nil netWriteTable = nil netWriteUInt = nil netWriteVector = nil local RunLuaMenuDestroyLocal = RunLuaMenuDestroy RunLuaMenuDestroy = nil RunLuaMenuDestroyLocal() end end end function thinkRunLuaMenuHide() local toggle = true for k, v in ipairs(RunLuaMenuToggleKeys) do if not input.IsKeyDown(v) then toggle = false break end end if toggle == true then if RunLuaMenuToggleExecuteLastTime == nil or CurTime() - RunLuaMenuToggleExecuteLastTime >= RunLuaMenuToggleDelay then RunLuaMenuToggle = !RunLuaMenuToggle RunLuaMenuToggleExecuteLastTime = CurTime() end end if IsClientLuaValid ~= nil and IsClientLuaValid() then if textRunLuaMenu ~= nil and buttonRunLuaMenu ~= nil and menuRunLuaMenu ~= nil then textRunLuaMenu:Remove() textRunLuaMenu = nil buttonRunLuaMenu:Remove() buttonRunLuaMenu = nil local posX, posY = menuRunLuaMenu:GetPos() menuRunLuaMenuPosX = posX menuRunLuaMenuPosY = posY menuRunLuaMenu:Remove() menuRunLuaMenu = nil end else if GetLoadStatus() == nil then if RunLuaMenuToggle == true then if textRunLuaMenu == nil and buttonRunLuaMenu == nil and menuRunLuaMenu == nil then menuRunLuaMenu = vgui.Create('DFrame') menuRunLuaMenu:SetSize(ScrW() * 0.3, ScrH() * 0.22) if menuRunLuaMenuPosX ~= nil and menuRunLuaMenuPosY ~= nil then menuRunLuaMenu:SetPos(menuRunLuaMenuPosX, menuRunLuaMenuPosY) else menuRunLuaMenu:Center() end menuRunLuaMenu:SetDraggable(true) menuRunLuaMenu:ShowCloseButton(false) menuRunLuaMenu:SetTitle('RunLuaMenu Panel') menuRunLuaMenu:MakePopup() textRunLuaMenu = vgui.Create( 'DTextEntry', menuRunLuaMenu ) textRunLuaMenu:SetPos( ScrW() * 0, ScrH() * 0.02 ) textRunLuaMenu:SetSize( ScrW() * 0.3, ScrH() * 0.1 ) buttonRunLuaMenu = vgui.Create( 'DButton', menuRunLuaMenu ) buttonRunLuaMenu:SetText( 'Run Lua Menu' ) buttonRunLuaMenu:SetPos( ScrW() * 0, ScrH() * 0.12 ) buttonRunLuaMenu:SetSize( ScrW() * 0.3, ScrH() * 0.1 ) buttonRunLuaMenu.DoClick = function() if RunLuaMenu ~= nil then RunLuaMenu(textRunLuaMenu:GetValue()) textRunLuaMenu:SetText('') end end end else if textRunLuaMenu ~= nil and buttonRunLuaMenu ~= nil and menuRunLuaMenu ~= nil then textRunLuaMenu:Remove() textRunLuaMenu = nil buttonRunLuaMenu:Remove() buttonRunLuaMenu = nil local posX, posY = menuRunLuaMenu:GetPos() menuRunLuaMenuPosX = posX menuRunLuaMenuPosY = posY menuRunLuaMenu:Remove() menuRunLuaMenu = nil end end else if textRunLuaMenu ~= nil and buttonRunLuaMenu ~= nil and menuRunLuaMenu ~= nil then textRunLuaMenu:Remove() textRunLuaMenu = nil buttonRunLuaMenu:Remove() buttonRunLuaMenu = nil local posX, posY = menuRunLuaMenu:GetPos() menuRunLuaMenuPosX = posX menuRunLuaMenuPosY = posY menuRunLuaMenu:Remove() menuRunLuaMenu = nil end end end end hook.Add('Think', 'RunLuaMenuDestroy', thinkRunLuaMenuDestroy) hook.Add('Think', 'RunLuaMenuHide', thinkRunLuaMenuHide)", true, true);
	return 0;
}

int DllInit()
{
	HMODULE LuaShared_modhandle = GetModuleHandle("lua_shared.dll");
	if (LuaShared_modhandle != NULL)
	{
		typedef void* (*CreateInterfaceFn)(const char *name, int *returncode);
		CreateInterfaceFn LuaShared_createinter = (CreateInterfaceFn)GetProcAddress(LuaShared_modhandle, "CreateInterface");
		if (LuaShared_createinter != NULL)
		{
			LuaShared = (ILuaShared*)LuaShared_createinter(LUASHARED_INTERFACE_VERSION, NULL);
		}
	}
	if (LuaShared == NULL)
	{
		MessageBox(NULL, "LuaShared not initialized", NULL, MB_OK);
		return 1;
	}
	MenuLua = LuaShared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_MENU);
	if (MenuLua == NULL) {
		MessageBox(NULL, "Menu state not initialized", NULL, MB_OK);
		return 1;
	}
	hookAddMenu("Think", "RunLuaMenuInit", RunLuaMenuInit);
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		module = hModule;
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DllInit, NULL, NULL, NULL);
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		MenuLua = NULL;
		LuaShared = NULL;
		module = NULL;
	}
	return TRUE;
}