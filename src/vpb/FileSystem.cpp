/* -*-c++-*- VirtualPlanetBuilder - Copyright (C) 1998-2007 Robert Osfield 
 *
 * This library is open source and may be redistributed and/or modified under  
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * OpenSceneGraph Public License for more details.
*/

#include <vpb/FileSystem>
#include <vpb/BuildLog>

using namespace vpb;
 
osg::ref_ptr<FileSystem>& FileSystem::instance()
{
    static osg::ref_ptr<FileSystem> s_FileSystem = new FileSystem;
    return s_FileSystem;
}

osgDB::FilePathList& vpb::getSourcePaths() { return FileSystem::instance()->getSourcePaths(); }
std::string& vpb::getDestinationDirectory() { return FileSystem::instance()->getDestinationDirectory(); }
std::string& vpb::getIntermediateDirectory() { return FileSystem::instance()->getIntermediateDirectory(); }
std::string& vpb::getLogDirectory() { return FileSystem::instance()->getLogDirectory(); }
std::string& vpb::getTaskDirectory() { return FileSystem::instance()->getTaskDirectory(); }
std::string& vpb::getMachineFileName() { return FileSystem::instance()->getMachineFileName(); }

FileSystem::FileSystem()
{
    _maxNumDatasets = getdtablesize();
    
    readEnvironmentVariables();
}

FileSystem::~FileSystem()
{
}

void FileSystem::readEnvironmentVariables()
{
    const char* str = getenv("VPB_SOURCE_PATHS");
    if (str) 
    {
        osgDB::convertStringPathIntoFilePathList(std::string(str),_sourcePaths);
    }

    str = getenv("VPB_DESTINATION_DIR");
    if (str) 
    {
        _destinationDirectory = str;
    }

    str = getenv("VPB_INTERMEDIATE_DIR");
    if (str) 
    {
        _intermediateDirectory = str;
    }

    str = getenv("VPB_LOG_DIR");
    if (str) 
    {
        _logDirectory = str;
    }

    str = getenv("VPB_TASK_DIR");
    if (str) 
    {
        _taskDirectory = str;
    }

    str = getenv("VPB_MACHINE_FILE");
    if (str) 
    {
        _machineFileName = str;
    }
}

void FileSystem::clearDatasetCache()
{
    _datasetMap.clear();
}

void FileSystem::clearUnusedDatasets(unsigned int numToClear)
{
    _datasetMap.clear();
}

GeospatialDataset* FileSystem::openGeospatialDataset(const std::string& filename)
{
    // first check to see if dataset already exists in cache, if so return it.
    DatasetMap::iterator itr = _datasetMap.find(filename);
    if (itr != _datasetMap.end()) return itr->second.get();

    // make sure there is room available for this new Dataset
    unsigned int numToClear = 10;
    if (_datasetMap.size()>=_maxNumDatasets) clearUnusedDatasets(numToClear);
    
    // double check to make sure there is room to open a new dataset
    if (_datasetMap.size()>=_maxNumDatasets)
    {
        log(osg::NOTICE,"Error: FileSystem::GDALOpen(%s) unable to open file as unsufficient file handles available.",filename.c_str());
        return 0;
    }
    
    // open the new dataset.
    GeospatialDataset* dataset = new GeospatialDataset(filename);

    // insert it into the cache
    _datasetMap[filename] = dataset;
    
    // return it.
    return dataset;
}
