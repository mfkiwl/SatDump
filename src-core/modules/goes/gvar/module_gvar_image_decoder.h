#pragma once

#include "module.h"
#include <complex>
#include <fstream>
#include "common/ctpl/ctpl_stl.h"
#include "image/infrared1_reader.h"
#include "image/infrared2_reader.h"
#include "image/visible_reader.h"

namespace goes
{
    namespace gvar
    {
        struct GVARImages
        {
            // Images used as a buffer when writing it out
            cimg_library::CImg<unsigned short> image1;
            cimg_library::CImg<unsigned short> image2;
            cimg_library::CImg<unsigned short> image3;
            cimg_library::CImg<unsigned short> image4;
            cimg_library::CImg<unsigned short> image5;
            int sat_number;
            int vis_width;
        };

        namespace events
        {
            struct GVARSaveChannelImagesEvent
            {
                GVARImages &images;
                std::tm *timeReadable;
                time_t timeUTC;
                std::string directory;
            };

            struct GVARSaveFCImageEvent
            {
                cimg_library::CImg<unsigned char> &false_color_image;
                int sat_number;
                std::tm *timeReadable;
                time_t timeUTC;
                std::string directory;
            };
        }

        class GVARImageDecoderModule : public ProcessingModule
        {
        protected:
            // Read buffer
            uint8_t *frame;

            std::ifstream data_in;
            std::atomic<size_t> filesize;
            std::atomic<size_t> progress;

            // Utils values
            bool isImageInProgress, isSavingInProgress;
            float approx_progess;

            // Image readers
            InfraredReader1 infraredImageReader1;
            InfraredReader2 infraredImageReader2;
            VisibleReader visibleImageReader;

            // Async image writing
            std::string directory;
            bool writeImagesAync = false;
            std::thread imageSavingThread;
            std::mutex imageVectorMutex;
            std::vector<GVARImages> imagesVector;
            void writeImages(GVARImages &images, std::string directory);
            void writeImagesThread();

            int nonEndCount, endCount;

            // Stats
            std::vector<int> scid_stats;
            std::vector<int> vis_width_stats, ir_width_stats;

            // UI Stuff
            unsigned int textureID = 0;
            uint32_t *textureBuffer;

        public:
            GVARImageDecoderModule(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters);
            ~GVARImageDecoderModule();
            static std::string getGvarFilename(int sat_number, std::tm *timeReadable, std::string channel);
            void process();
            void drawUI(bool window);
            std::vector<ModuleDataType> getInputTypes();
            std::vector<ModuleDataType> getOutputTypes();

        public:
            static std::string getID();
            virtual std::string getIDM() { return getID(); };
            static std::vector<std::string> getParameters();
            static std::shared_ptr<ProcessingModule> getInstance(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters);
        };
    } // namespace elektro_arktika
}