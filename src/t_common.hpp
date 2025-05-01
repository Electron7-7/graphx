#ifndef GRAPHX_THEATRE_FILE_FORMAT
#include <memory>
#include <string>
#define GRAPHX_THEATRE_FILE_FORMAT

#ifdef COMPILER_FORWARD_DECLARATIONS // This is to keep forward declarations from causing issues when including header files
struct Theatre;
#endif

void I_LoadNewMainTheatre(long TheatreUID);
void I_LoadChildTheatre(long TheatreUID, std::shared_ptr<Theatre> ParentTheatre);
void I_EmbedExternalTheatre(const std::string& GraphXTheatreFilePath);
bool I_CheckForAndLoadExternalTheatres();
#endif
