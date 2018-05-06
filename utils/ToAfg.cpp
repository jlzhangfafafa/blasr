#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <HDFPlsReader.hpp>
#include <HDFRegionTableReader.hpp>
#include <SMRTSequence.hpp>
#include <amos/AfgBasWriter.hpp>
#include <files/ReaderAgglomerate.hpp>
#include <reads/ReadInterval.hpp>
#include <reads/RegionTable.hpp>
#include <utils.hpp>
#include <utils/FileOfFileNames.hpp>
#include <utils/RegionUtils.hpp>

void PrintUsage()
{
    std::cout << "usage: toAfg input.filetype output.filetype" << std::endl
              << "                                         [-minSubreadLength l] " << std::endl
              << "                                         [-regionTable regions_file] "
              << std::endl
              << "                                         [-noSplitSubreads]" << std::endl
              << "                                         [-useccsdenovo]" << std::endl
              << "                                         [-uniformQV QV]" << std::endl
              << "Print reads stored in a file (pls|fasta|fastq) as an afg." << std::endl;
}

int main(int argc, char* argv[])
{

    std::string inputFileName, outputFileName;

    if (argc < 2) {
        PrintUsage();
        std::exit(EXIT_FAILURE);
    }
    std::vector<std::string> inputFileNames;
    inputFileName = argv[1];
    outputFileName = argv[2];
    int argi = 3;
    RegionTable regionTable;
    std::string regionsFOFNName = "";
    std::vector<std::string> regionFileNames;
    bool splitSubreads = true;
    bool useCCS = false;
    bool useUniformQV = false;
    int uniformQV = 7;
    int minSubreadLength = 1;
    while (argi < argc) {
        if (strcmp(argv[argi], "-regionTable") == 0) {
            regionsFOFNName = argv[++argi];
        } else if (strcmp(argv[argi], "-noSplitSubreads") == 0) {
            splitSubreads = false;
        } else if (strcmp(argv[argi], "-minSubreadLength") == 0) {
            minSubreadLength = atoi(argv[++argi]);
        } else if (strcmp(argv[argi], "-useccsdenovo") == 0) {
            useCCS = true;
        } else if (strcmp(argv[argi], "-uniformQV") == 0) {
            useUniformQV = true;
            uniformQV = atoi(argv[++argi]);
        } else {
            PrintUsage();
            std::cout << "ERROR! Option " << argv[argi] << " is not supported." << std::endl;
        }
        argi++;
    }

    if (FileOfFileNames::IsFOFN(inputFileName)) {
        FileOfFileNames::FOFNToList(inputFileName, inputFileNames);
    } else {
        inputFileNames.push_back(inputFileName);
    }
    if (regionsFOFNName == "") {
        regionFileNames = inputFileNames;
    } else {
        if (FileOfFileNames::IsFOFN(regionsFOFNName)) {
            FileOfFileNames::FOFNToList(regionsFOFNName, regionFileNames);
        } else {
            regionFileNames.push_back(regionsFOFNName);
        }
    }

    std::ofstream fastaOut;
    CrucialOpen(outputFileName, fastaOut);
    HDFRegionTableReader hdfRegionReader;
    AfgBasWriter afgWriter;
    if (useUniformQV) {
        afgWriter.SetDefaultQuality(uniformQV);
    }

    afgWriter.Initialize(outputFileName);

    for (size_t plsFileIndex = 0; plsFileIndex < inputFileNames.size(); plsFileIndex++) {
        if (splitSubreads) {
            hdfRegionReader.Initialize(regionFileNames[plsFileIndex]);
            hdfRegionReader.ReadTable(regionTable);
        }

        ReaderAgglomerate reader;
        // reader.SkipReadQuality(); // should have been taken care of by *Filter modules
        if (useCCS) {
            reader.UseCCS();
        } else {
            reader.IgnoreCCS();
        }
        reader.Initialize(inputFileNames[plsFileIndex]);
        CCSSequence seq;
        int seqIndex = 0;
        std::vector<ReadInterval> subreadIntervals;
        while (reader.GetNext(seq)) {
            ++seqIndex;

            if (useUniformQV && seq.qual.data != NULL) {
                for (DNALength qvIndex = 0; qvIndex < seq.length; qvIndex++) {
                    seq.qual[qvIndex] = uniformQV;
                }
            }

            if (splitSubreads == false) {
                if (seq.length >= static_cast<DNALength>(minSubreadLength)) {
                    afgWriter.Write(seq);
                }
                seq.Free();
                continue;
            }

            DNALength hqReadStart, hqReadEnd;
            int score;
            GetReadTrimCoordinates(seq, seq.zmwData, regionTable, hqReadStart, hqReadEnd, score);

            if (regionTable.HasHoleNumber(seq.HoleNumber())) {
                subreadIntervals =
                    regionTable[seq.HoleNumber()].SubreadIntervals(seq.length, true, true);
            } else {
                subreadIntervals = {};
            }

            if (seq.length == 0 and subreadIntervals.size() > 0) {
                std::cout
                    << "WARNING! A high quality interval region exists for a read of length 0."
                    << std::endl;
                std::cout << "  The offending ZMW number is " << seq.HoleNumber() << std::endl;
                seq.Free();
                continue;
            }

            for (size_t intvIndex = 0; intvIndex < subreadIntervals.size(); intvIndex++) {
                SMRTSequence subreadSequence;

                DNALength subreadStart =
                    static_cast<DNALength>(subreadIntervals[intvIndex].start) > hqReadStart
                        ? static_cast<DNALength>(subreadIntervals[intvIndex].start)
                        : hqReadStart;
                DNALength subreadEnd =
                    static_cast<DNALength>(subreadIntervals[intvIndex].end) < hqReadEnd
                        ? static_cast<DNALength>(subreadIntervals[intvIndex].end)
                        : hqReadEnd;
                DNALength subreadLength = subreadEnd - subreadStart;

                if (subreadLength < DNALength(minSubreadLength)) continue;

                subreadSequence.SubreadStart(subreadStart);
                subreadSequence.SubreadEnd(subreadEnd);
                subreadSequence.ReferenceSubstring(seq, subreadStart, subreadLength);

                std::stringstream titleStream;
                titleStream << seq.title << "/" << subreadIntervals[intvIndex].start << "_"
                            << subreadIntervals[intvIndex].end;
                subreadSequence.CopyTitle(titleStream.str());
                afgWriter.Write(subreadSequence);
            }
            seq.Free();
        }
        reader.Close();
        hdfRegionReader.Close();
    }
}
