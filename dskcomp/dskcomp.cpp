////////////////////////////////////////////
//
// DskComp - A small utility for comparing
//           disk images in Marco Vieth and
//           Kevin Thacker's DSK format.
//
// By Roberto Carlos Fernández Gerhardt
//
////////////////////////////////////////////
#define CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <cstring>
#include "dskfile.h"

using namespace std;

// Compare disk images and show output to the console
bool Compare( CDSKFile& disk1, CDSKFile& disk2, bool _verbose )
{
    const CDSKFile_DiskInfoBlock& info1 = disk1.GetDiskInfoBlock();
    const CDSKFile_DiskInfoBlock& info2 = disk2.GetDiskInfoBlock();

    // Geometry checks
    // 1 - Number of sides
    if( info1.sidesNum != info2.sidesNum )
    {
        if( _verbose )
        {
            cout << "Disks have different number of sides. " << info1.sidesNum << " vs ";
            cout << info2.sidesNum << "." << endl;
        }
        return false;
    }

    for( int side = 0; side < info1.sidesNum; ++side )
    {
        const vector<CDSKFile_TrackInfoBlock>& disk1Side = disk1.GetSide(side != 0);
        const vector<CDSKFile_TrackInfoBlock>& disk2Side = disk2.GetSide(side != 0);

        // 2 - Number of tracks
        if( disk1Side.size() != disk2Side.size() )
        {
            if( _verbose )
            {
                cout << "Disks side " << side << " have different number of tracks. ";
                cout << disk1Side.size() << " vs " << disk2Side.size() << "." << endl;
            }
            return false;
        }

        size_t numTracks = disk1Side.size();

        for( size_t track = 0; track < numTracks; ++track )
        {
            // 3 - Number of sectors
            if( disk1Side[track].sectorsNum != disk2Side[track].sectorsNum )
            {
                if( _verbose )
                {
                    cout << "Disks side " << side << ", track " << track << " have different number of sectors. ";
                    cout << disk1Side[track].sectorsNum << " vs " << disk2Side[track].sectorsNum << "." << endl;
                }
                return false;
            }

            // 4 - Sector info and data
            size_t sectorsNum = disk1Side[track].sectorsNum;

            for( size_t sector = 0; sector < sectorsNum; ++sector )
            {
                // Sector info
                if( sector >= disk1Side[track].sectorInfoList.size() ||
                    sector >= disk2Side[track].sectorInfoList.size() )
                {
                    continue;
                }

                if( disk1Side[track].sectorInfoList[sector].track      != disk2Side[track].sectorInfoList[sector].track      ||
                    disk1Side[track].sectorInfoList[sector].side       != disk2Side[track].sectorInfoList[sector].side       ||
                    disk1Side[track].sectorInfoList[sector].sectorID   != disk2Side[track].sectorInfoList[sector].sectorID   ||
                    disk1Side[track].sectorInfoList[sector].sectorSize != disk2Side[track].sectorInfoList[sector].sectorSize ||
                    disk1Side[track].sectorInfoList[sector].FDCStatus1 != disk2Side[track].sectorInfoList[sector].FDCStatus1 ||
                    disk1Side[track].sectorInfoList[sector].FDCStatus2 != disk2Side[track].sectorInfoList[sector].FDCStatus2 ||
                    disk1Side[track].sectorInfoList[sector].dataLength != disk2Side[track].sectorInfoList[sector].dataLength )
                {
                    if( _verbose )
                    {
                        cout << "Disks side " << side << ", track " << track << ", sector " << sector;
                        cout << " have different sector info. " << endl;
                        cout << "track      " << disk1Side[track].sectorInfoList[sector].track      << " vs " << disk2Side[track].sectorInfoList[sector].track      << endl;
                        cout << "side       " << disk1Side[track].sectorInfoList[sector].side       << " vs " << disk2Side[track].sectorInfoList[sector].side       << endl;
                        cout << "sectorID   " << disk1Side[track].sectorInfoList[sector].sectorID   << " vs " << disk2Side[track].sectorInfoList[sector].sectorID   << endl;
                        cout << "sectorSize " << disk1Side[track].sectorInfoList[sector].sectorSize << " vs " << disk2Side[track].sectorInfoList[sector].sectorSize << endl;
                        cout << "FDCStatus1 " << disk1Side[track].sectorInfoList[sector].FDCStatus1 << " vs " << disk2Side[track].sectorInfoList[sector].FDCStatus1 << endl;
                        cout << "FDCStatus2 " << disk1Side[track].sectorInfoList[sector].FDCStatus2 << " vs " << disk2Side[track].sectorInfoList[sector].FDCStatus2 << endl;
                        cout << "dataLength " << disk1Side[track].sectorInfoList[sector].dataLength << " vs " << disk2Side[track].sectorInfoList[sector].dataLength << endl;
                    }
                    return false;
                }

                // Sector data
                size_t dataLength = disk1Side[track].sectorInfoList[sector].dataLength;
                if( 0 != memcmp( disk1Side[track].sectorData[sector].data(), disk2Side[track].sectorData[sector].data(), dataLength) )
                {
                    if( _verbose )
                    {
                        cout << "Disks side " << side << ", track " << track << ", sector " << sector;
                        cout << " have different sector data. " << endl;
                    }
                    return false;
                }
            }
        }
    }

    return true;
}

void ShowUsage()
{
    cout << "DskComp Usage:" << endl << endl;
    cout << "    DskComp filename1 filename2 [options]" << endl << endl;
    cout << "    Options:" << endl;
    cout << "        -v" << endl << "            Verbose, lots of info." << endl << endl;
    cout << "DskComp returns with 0 if the disk images have the same contents, nonzero otherwise." << endl << endl;
}

int main(int argc, char** argv)
{
    // Variables
    bool verbose = false;
    CDSKFile disk1;
    CDSKFile disk2;

    // Check arguments
    if( argc < 3 ) // Program name + two disk image file names
    {
        cout << "Missing arguments. Need at least two valid filenames." << endl << endl;
        ShowUsage();
        return -1;
    }

    for( int iArg = 0; iArg < argc; ++iArg )
    {
        if( 0 == strcmp(argv[iArg],"-v") ) verbose = true;
    }

    // Load disk images
    if( !disk1.Load(argv[1]) )
    {
        cout << "Couldn't load " << argv[1] << endl;
        return -1;
    }

    if( !disk2.Load(argv[2]) )
    {
        cout << "Couldn't load " << argv[2] << endl;
        return -1;
    }

    // Compare images
    if( !Compare(disk1, disk2, verbose) )
    {
        return -1;
    }

    // Good bye!
    return 0;
}
