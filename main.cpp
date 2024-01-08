#include <mod/amlmod.h>
#include <mod/logger.h>
#include <mod/config.h>

#if !defined(IAML_VER) && IAML_VER < 1020100
    #error "You need to update your MOD folder to 1.2.1!"
#endif

MYMODCFG(net.rusjj.jpatch, JPatch, 1.5, RusJJ)
BEGIN_DEPLIST()
    ADD_DEPENDENCY_VER(net.rusjj.aml, 1.2.1)
END_DEPLIST()

/////////////////////////////////////////////////////////////////////////////
///////////////////////////////     Saves     ///////////////////////////////
/////////////////////////////////////////////////////////////////////////////
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
    cfg->Bind("IdeasFrom", "", "About")->SetString("MTA:SA Team, re3 contributors, JuniorDjjr, ThirteenAG, Blackbird88, 0x416c69, Whitetigerswt, XMDS, Peepo"); cfg->ClearLast();
    cfg->Bind("Discord", "", "About")->SetString("https://discord.gg/2MY7W39kBg"); cfg->ClearLast();
    cfg->Bind("GitHub", "", "About")->SetString("https://github.com/AndroidModLoader/JPatch"); cfg->ClearLast();
    cfg->Save();

    pGTASA = aml->GetLib("libGTASA.so");
    hGTASA = aml->GetLibHandle("libGTASA.so");

    pGTAVC = aml->GetLib("libGTAVC.so");
    hGTAVC = aml->GetLibHandle("libGTAVC.so");
    
    pSC = aml->GetLib("libSCAnd.so");
    hSC = aml->GetLibHandle("libSCAnd.so");
    
    if(pGTASA) GTA_SA::JPatch();
    else if(pGTAVC) GTA_VC::JPatch();
}