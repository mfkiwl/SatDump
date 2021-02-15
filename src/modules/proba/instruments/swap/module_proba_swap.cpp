#include "module_proba_swap.h"
#include <fstream>
#include "swap_reader.h"
#include "modules/proba/ccsds/demuxer.h"
#include "modules/proba/ccsds/vcdu.h"
#include "logger.h"
#include <filesystem>

#define WRITE_IMAGE_LOCAL(image, path)         \
    image.save_png(std::string(path).c_str()); \
    all_images.push_back(path);

// Return filesize
size_t getFilesize(std::string filepath);

namespace proba
{
    namespace swap
    {
        ProbaSWAPDecoderModule::ProbaSWAPDecoderModule(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters) : ProcessingModule(input_file, output_file_hint, parameters)
        {
        }

        void ProbaSWAPDecoderModule::process()
        {
            size_t filesize = getFilesize(d_input_file);
            std::ifstream data_in(d_input_file, std::ios::binary);

            std::string directory = d_output_file_hint.substr(0, d_output_file_hint.rfind('/')) + "/SWAP";

            if (!std::filesystem::exists(directory))
                std::filesystem::create_directory(directory);

            logger->info("Using input frames " + d_input_file);
            logger->info("Decoding to " + directory);

            time_t lastTime = 0;

            uint8_t buffer[1279];

            // CCSDS Demuxer
            libccsds::Demuxer ccsdsDemuxer(1103, false);

            SWAPReader swap_reader(directory);

            logger->info("Demultiplexing and deframing...");

            while (!data_in.eof())
            {
                // Read buffer
                data_in.read((char *)buffer, 1279);

                int vcid = libccsds::parseVCDU(buffer).vcid;

                if (vcid == 3)
                {
                    std::vector<libccsds::CCSDSPacket> pkts = ccsdsDemuxer.work(buffer);

                    if (pkts.size() > 0)
                    {
                        for (libccsds::CCSDSPacket pkt : pkts)
                        {
                            if (pkt.header.apid == 2047)
                                continue;

                            if (pkt.header.apid == 20)
                            {
                                swap_reader.work(pkt);
                            }
                        }
                    }
                }

                if (time(NULL) % 10 == 0 && lastTime != time(NULL))
                {
                    lastTime = time(NULL);
                    logger->info("Progress " + std::to_string(round(((float)data_in.tellg() / (float)filesize) * 1000.0f) / 10.0f) + "%");
                }
            }

            data_in.close();

            swap_reader.save();

            d_output_files = swap_reader.all_images;
        }

        std::string ProbaSWAPDecoderModule::getID()
        {
            return "proba_swap";
        }

        std::vector<std::string> ProbaSWAPDecoderModule::getParameters()
        {
            return {};
        }

        std::shared_ptr<ProcessingModule> ProbaSWAPDecoderModule::getInstance(std::string input_file, std::string output_file_hint, std::map<std::string, std::string> parameters)
        {
            return std::make_shared<ProbaSWAPDecoderModule>(input_file, output_file_hint, parameters);
        }
    } // namespace swap
} // namespace proba