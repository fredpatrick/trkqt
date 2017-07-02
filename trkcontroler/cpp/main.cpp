#include "trkcontroler.h"
#include "jobclock.h"
#include "trkutl.h"
#include "filestore.h"
#include "debugcntl.h"
#include <QApplication>
#include <QSurfaceFormat>

#include <iostream>
#include <sstream>

using namespace trk;

int main(int argc, char *argv[]) 
{
    JobClock* jbclk = JobClock::instance();
    std::string dbg_ts = jbclk->tod_timestamp();
    std::string homedir     = "/Users/fredpatrick/";
    std::string logdir      = "wrk/log/";
    std::string dbgfil_name = homedir + logdir +  dbg_ts + "_TrkControler.txt";
    std::string evtfil_name = homedir + logdir +  dbg_ts + "_EventLog.txt";
    trk::FileStore* fs = trk::FileStore::instance();
    fs->vtxfil("/Users/fredpatrick/wrk/cfg/layout24c.vtx");
    fs->cfgfil("/Users/fredpatrick/wrk/cfg/layout_config.txt");
    fs->pthfil("/Users/fredpatrick/wrk/cfg/path_definition.txt");
    fs->dbgfil(dbgfil_name);
    fs->evtfil(evtfil_name);
    trk::DebugCntl* dbg = trk::DebugCntl::instance();
    int debug_level = trk::debug_level(argc, argv);
    dbg->level(debug_level);
    *dbg << "TrkControler, debug level = " << debug_level << trk::endl;
    std::ostringstream ss;
    ss << *fs;
    *dbg<< ss.str();
    std::ofstream ostrm("/Users/fredpatrick/wrk/log/lognam");
    ostrm << dbgfil_name << std::endl;
    ostrm.close();
    QApplication a(argc, argv);
    QSurfaceFormat( format);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(4,3);
    QSurfaceFormat::setDefaultFormat(format);
    TrkControler w;
    w.show();

    return a.exec();
}
