

#include <utils/FFL_File.hpp>


namespace FFL {
    
	FFL::File*  gTrackFile;
    void FFL_setTrackFilePath(const char* path){
        if(path==NULL || path[0] ==0 ){
            return;
        }
        
        if (gTrackFile != NULL) {
            gTrackFile->close();
            FFL_SafeFree(gTrackFile);
   
        }
        
        gTrackFile = new FFL::File();
        gTrackFile->create(path);

    }
    
	void postTraceinfo(const String& info) {
        if (gTrackFile) {
			gTrackFile->write((void*)info.c_str(), info.size(), 0);
			gTrackFile->write((void*)"\r", 1, 0);
		}
		//FFL_LOG_INFO_TAG("track", info.c_str());
	}
}
