#pragma once

class ILuaInterface;
struct lua_State
{
	unsigned char    _ud[69];
	ILuaInterface*    m_interface;
};

struct UserData {
	void*            data;
	unsigned char    type;
};

namespace Type
{
	enum
	{
		INVALID = -1,
		NIL,
		BOOL,
		LIGHTUSERDATA,
		NUMBER,
		STRING,
		TABLE,
		FUNCTION,
		USERDATA,
		THREAD,

		// UserData
		ENTITY,
		VECTOR,
		ANGLE,
		PHYSOBJ,
		SAVE,
		RESTORE,
		DAMAGEINFO,
		EFFECTDATA,
		MOVEDATA,
		RECIPIENTFILTER,
		USERCMD,
		SCRIPTEDVEHICLE,

		// Client Only
		MATERIAL,
		PANEL,
		PARTICLE,
		PARTICLEEMITTER,
		TEXTURE,
		USERMSG,

		CONVAR,
		IMESH,
		MATRIX,
		SOUND,
		PIXELVISHANDLE,
		DLIGHT,
		VIDEO,
		FILE,

		COUNT
	};

	static const char* Name[] =
	{
		"nil",
		"bool",
		"lightuserdata",
		"number",
		"string",
		"table",
		"function",
		"userdata",
		"thread",
		"entity",
		"vector",
		"angle",
		"physobj",
		"save",
		"restore",
		"damageinfo",
		"effectdata",
		"movedata",
		"recipientfilter",
		"usercmd",
		"vehicle",
		"material",
		"panel",
		"particle",
		"particleemitter",
		"texture",
		"usermsg",
		"convar",
		"mesh",
		"matrix",
		"sound",
		"pixelvishandle",
		"dlight",
		"video",
		"file",

		0
	};
};

typedef int(__cdecl* CLuaFunction)(lua_State*);

enum
{
	SPECIAL_GLOB,
	SPECIAL_ENV,
	SPECIAL_REG,
};

class ILuaInterface
{
public:
	virtual int             Top(void) = 0;
	virtual void            Push(int iStackPos) = 0;
	virtual void            Pop(int iAmt = 1) = 0;
	virtual void            GetTable(int iStackPos) = 0;
	virtual void            GetField(int iStackPos, const char* strName) = 0;
	virtual void            SetField(int iStackPos, const char* strName) = 0;
	virtual void            CreateTable() = 0;
	virtual void            SetTable(int i) = 0;
	virtual void            SetMetaTable(int i) = 0;
	virtual bool            GetMetaTable(int i) = 0;
	virtual void            Call(int iArgs, int iResults) = 0;
	virtual int             PCall(int iArgs, int iResults, int iErrorFunc) = 0;
	virtual int             Equal(int iA, int iB) = 0;
	virtual int             RawEqual(int iA, int iB) = 0;
	virtual void            Insert(int iStackPos) = 0;
	virtual void            Remove(int iStackPos) = 0;
	virtual int             Next(int iStackPos) = 0;
	virtual void*           NewUserdata(unsigned int iSize) = 0;
	virtual void            ThrowError(const char* strError) = 0;
	virtual void            CheckType(int iStackPos, int iType) = 0;
	virtual void            ArgError(int iArgNum, const char* strMessage) = 0;
	virtual void            RawGet(int iStackPos) = 0;
	virtual void            RawSet(int iStackPos) = 0;
	virtual const char*     GetString(int iStackPos = -1, unsigned int* iOutLen = 0) = 0;
	virtual double          GetNumber(int iStackPos = -1) = 0;
	virtual bool            GetBool(int iStackPos = -1) = 0;
	virtual CLuaFunction    GetCFunction(int iStackPos = -1) = 0;
	virtual void*           GetUserdata(int iStackPos = -1) = 0;
	virtual void            PushNil() = 0;
	virtual void            PushString(const char* val, unsigned int iLen = 0) = 0;
	virtual void            PushNumber(double val) = 0;
	virtual void            PushBool(bool val) = 0;
	virtual void            PushCFunction(CLuaFunction val) = 0;
	virtual void            PushCClosure(CLuaFunction val, int iVars) = 0;
	virtual void            PushUserdata(void*) = 0;
	virtual int             ReferenceCreate() = 0;
	virtual void            ReferenceFree(int i) = 0;
	virtual void            ReferencePush(int i) = 0;
	virtual void            PushSpecial(int iType) = 0;
	virtual bool            IsType(int iStackPos, int iType) = 0;
	virtual int             GetType(int iStackPos) = 0;
	virtual const char*     GetTypeName(int iType) = 0;
	virtual void            CreateMetaTableType(const char* strName, int iType) = 0;
	virtual const char*     CheckString(int iStackPos = -1) = 0;
	virtual double          CheckNumber(int iStackPos = -1) = 0;
	virtual size_t          ObjLen(int index) = 0;
	virtual void			unk46(void) = 0;
	virtual void			unk47(void) = 0;
	virtual void			unk48(void) = 0;
	virtual void			unk49(void) = 0;
	virtual void			unk50(void) = 0;
	virtual void			unk51(void) = 0;
	virtual void			unk52(void) = 0;
	virtual void			unk53(void) = 0;
	virtual void			unk54(void) = 0;
	virtual void			unk55(void) = 0;
	virtual void			unk56(void) = 0;
	virtual void			unk57(void) = 0;
	virtual void			unk58(void) = 0;
	virtual void			unk59(void) = 0;
	virtual void			unk60(void) = 0;
	virtual void			unk61(void) = 0;
	virtual void			unk62(void) = 0;
	virtual void			unk63(void) = 0;
	virtual void			unk64(void) = 0;
	virtual void			unk65(void) = 0;
	virtual void			unk66(void) = 0;
	virtual void			unk67(void) = 0;
	virtual void			unk68(void) = 0;
	virtual void			unk69(void) = 0;
	virtual void			unk70(void) = 0;
	virtual void			unk71(void) = 0;
	virtual void			unk72(void) = 0;
	virtual void			unk73(void) = 0;
	virtual void			unk74(void) = 0;
	virtual void			unk75(void) = 0;
	virtual void			unk76(void) = 0;
	virtual void			unk77(void) = 0;
	virtual void			unk78(void) = 0;
	virtual void			unk79(void) = 0;
	virtual void			unk80(void) = 0;
	virtual void			unk81(void) = 0;
	virtual void			unk82(void) = 0;
	virtual void			unk83(void) = 0;
	virtual void			unk84(void) = 0;
	virtual void			unk85(void) = 0;
	virtual void			unk86(void) = 0;
	virtual void			unk87(void) = 0;
	virtual void			unk88(void) = 0;
	virtual void			unk89(void) = 0;
	virtual void			unk90(void) = 0;
	virtual void			unk91(void) = 0;
	virtual bool			RunString(const char* filename, const char* path, const char* stringToRun, bool run = true, bool showErrors = true) = 0;

};

enum LuaInterfaceType
{
	LUAINTERFACE_CLIENT = 0,
	LUAINTERFACE_SERVER = 1,
	LUAINTERFACE_MENU = 2
};

class ILuaShared
{
public:
	virtual void            Destructor();
	virtual void            Init(void*, bool, void* *, void*);
	virtual void            Shutdown(void);
	virtual void            DumpStats(void);
	virtual ILuaInterface*    CreateLuaInterface(LuaInterfaceType type, bool);
	virtual int                CloseLuaInterface(ILuaInterface*);
	virtual ILuaInterface*    GetLuaInterface(LuaInterfaceType type);
	virtual void            LoadFile(void*, void*, bool, bool);
	virtual void            GetCache(void*);
	virtual void            MountLua(char  const*);
	virtual void            MountLuaAdd(char  const*, char  const*);
	virtual void            UnMountLua(char  const*);
	virtual void            SetFileContents(char  const*, char  const*);
	virtual void            SetLuaFindHook(void *);
	virtual void            FindScripts(void*, void*, void*);
};

#define LUASHARED_INTERFACE_VERSION "LUASHARED003"