#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#include <sys/system_properties.h>

#if !defined(IAML_VER) && IAML_VER < 1020100
    #error "You need to update your MOD folder to 1.2.1!"
#endif

MYMODCFG(net.rusjj.jpatch, JPatch, 1.8.2, RusJJ)
BEGIN_DEPLIST()
    ADD_DEPENDENCY_VER(net.rusjj.aml, 1.2.1)
END_DEPLIST()

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Saves     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int androidSdkVer = 0;
uintptr_t pGTASA, pGTAVC, pSC;
void *hGTASA, *hGTAVC, *hSC;

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Funcs     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
namespace GTA_SA { void JPatch(); };
namespace GTA_VC { void JPatch(); };

/*extern "C" const char* OnUpdaterURLRequested()
{
    return "https://raw.githubusercontent.com/AndroidModLoader/JPatch/main/updater.txt";
}*/
extern "C" void OnModLoad()
{
    logger->SetTag("JPatch");
    
    cfg->Bind("Author", "", "About")->SetString("[-=KILL MAN=-]"); cfg->ClearLast();
    cfg->Bind("Discord", "", "About")->SetString("https://discord.gg/2MY7W39kBg"); cfg->ClearLast();
    cfg->Bind("GitHub", "", "About")->SetString("https://github.com/AndroidModLoader/JPatch"); cfg->ClearLast();

    pGTASA = aml->GetLib("libGTASA.so");
    hGTASA = aml->GetLibHandle("libGTASA.so");

    pGTAVC = aml->GetLib("libGTAVC.so");
    hGTAVC = aml->GetLibHandle("libGTAVC.so");
    
    pSC = aml->GetLib("libSCAnd.so");
    hSC = aml->GetLibHandle("libSCAnd.so");

    char sdk_ver_str[92]; // PROPERTY_VALUE_MAX
    if(__system_property_get("ro.build.version.sdk", sdk_ver_str))
    {
        androidSdkVer = atoi(sdk_ver_str);
    }
    
    if(pGTASA) GTA_SA::JPatch();
    else if(pGTAVC) GTA_VC::JPatch();

    cfg->Save();
}
