/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011-2015 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "emptyPolyPatch.H"
#include "IFstream.H"
#include "stringOps.H"

int main(int argc, char *argv[])
{
    // Initialise OF case
    #include "setRootCase.H"

    // These two create the time system (instance called runTime) and fvMesh (instance called mesh).
    #include "createTime.H"
    #include "createMesh.H"

    // ---
    // Get access to a custom dictionary
    dictionary customDict;
    const word dictName("customProperties");

    // return case folder dir and case name respectively
    Info<< "args.rootPath(): " << args.rootPath() << endl
        << "args.caseName()" << args.caseName() << endl;

    // some code try to understant instance meaning
    // Info<< mesh.time().findInstance(mesh.time().path(), "reconstruct_0") << endl;
    // Info<< mesh.time().findInstancePath(instant("40")) << endl;
    // instantList timeDirs_ = findTimes(mesh.time().path());

    Info<< readDir(mesh.time().rootPath(), fileName::DIRECTORY) << endl;
    // output ----------
    // 4
    // (
    // "postProcessing"
    // "constant"
    // "system"
    // "0"
    // )

    // Info<< "timeDirs_: " << timeDirs_ << endl;
    // mkDir("test");
    // fileNameList dirEntries(Foam::readDir(directory, fileName::DIRECTORY));
    // instantList times = sortTimes(dirEntries, constantName);

    // Create and input-output object - this holds the path to the dict and its name
    IOobject dictIO
    (
        dictName, // name of the file
        mesh.time().constant(), // path to where the file is
        mesh, // reference to the mesh needed by the constructor
        IOobject::MUST_READ // indicate that reading this dictionary is compulsory
    );

    // Check the if the dictionary is present and follows the OF format
    // if (!dictIO.typeHeaderOk<dictionary>(true))
    //     FatalErrorIn(args.executable()) << "Cannot open specified refinement dictionary "
    //         << dictName << exit(FatalError);

    // Initialise the dictionary object
    customDict = IOdictionary(dictIO);

    // ---
    // Read various pieces of information from the main part of the dictionary

    // Lookup which does not need to be told what type of variable we're looking for and
    // uses the standard C++ stringstream syntax
    word someWord;
    customDict.lookup("someWord") >> someWord;
    // Info << "hello Rsh/n" << someWord << nl;

    // This template method needs to know the type of the variable and can provide
    // a default value if the entry is not found in the dictionary
    scalar someScalar( customDict.lookupOrDefault<scalar>("someScalar", 1.0) );

    // A switch is a neat feature allowing boolean values to be read from a dict,
    // it supports the OpenFOAM yes/on/true/1 and no/off/false/0 values automatically.
    bool someBool ( customDict.lookupOrDefault<Switch>("someBool",true) );

    // Lists of values may also be read in the same way
    List<scalar> someList ( customDict.lookup("someList") );

    // This type of container is particularly interesting - it associates entries with
    // given key values (here of word type but can be anything); useful when
    // associating things by indices in a list is less handy
    HashTable<vector,word> someHashTable ( customDict.lookup("someHashTable") );

    // Summarise what's been read and print in the console
    Info << nl << "Read the following:" << nl << nl
         << "someWord " << someWord << nl << nl
         << "someScalar " << someScalar << nl << nl
         << "someList " << someList << nl << nl
         << "someHashTable " << someHashTable << nl << nl
         << "someBool " << someBool << nl << nl
         << endl;

    // ---
    // Create a custom directory and write an output file

    // Create the output path directory
    fileName outputDir = mesh.time().path()/"postProcessing";
    // "/mnt/d/Learning/RshOpenFOAMSourceCode/BasicOpenFOAMProgrammingTutorials/OFtutorial01_inputOutput/testCase"
    Info << mesh.time().path() << nl;
    // 0
    Info << runTime.timeName() << endl;
    // Creathe the directory
    mkDir(outputDir);

    // File pointer to direct the output to
	autoPtr<OFstream> outputFilePtr;
    // Open the file in the newly created directory
    outputFilePtr.reset(new OFstream(outputDir/"customOutputFile.dat"));

    // Write stuff
    outputFilePtr() << "# This is a header" << endl;
    outputFilePtr() << "0 1 2 3 4 5" << endl;

    // Append to the imported hash table and wirte it too
    someHashTable.insert("newKey", vector(1., 0., 0.));
    outputFilePtr() << someHashTable << endl;

    
    const polyPatch patch 
    (
        // name,            // origin word, but has compilation error, name is not defined
        "name",
        0,                  // size
        mesh.nFaces(),      // start
        mesh.boundaryMesh().size(),  // index
        mesh.boundaryMesh(),// polyBoundaryMesh
        polyPatch::typeName
    );

    Info<< "PolyPatch::typeName: " << patch.typeName << endl;

    Info<< "End\n" << endl;

    // fileName dataPath (mesh.time().path()/"postProcessing");

    // // List<word> modeNumber ({"0", "1", "2", "3", "4"});
    // List<word> modeNumber ({"0", "1", "2"});
    // List<word> fieldName ({"Ux", "Uy", "Uz", "magU", "gradp0", "gradp1", "gradp2"});

    // forAll(fieldName, nameNo)
    // {
    //     Info << "FieldName: " << fieldName[nameNo] << endl;

    //     forAll(modeNumber, No_)
    //     {
    //         fileName dataFile (dataPath/fieldName[nameNo] + "_mode" + modeNumber[No_]);
    //         fileName modeFieldName(fieldName[nameNo] + "_mode" + modeNumber[No_]);

    //         volScalarField Uz_mode
    //         (
    //             IOobject
    //             (
    //                 modeFieldName,
    //                 mesh.time().timeName(),
    //                 mesh,
    //                 IOobject::NO_READ,
    //                 IOobject::AUTO_WRITE
    //             ),
    //             mesh,
    //             dimVelocity
    //         );

    //         IFstream dataStream(dataFile);

    //         forAll(mesh.cellZones()[0], cellI)
    //         {
    //             forAll(mesh.cellZones(), zoneI)
    //             {
    //                 label cell = mesh.cellZones()[zoneI][cellI];
    //                 dataStream.read(Uz_mode[cell]);
    //             }
    //         }

    //         Uz_mode.write();
    //     }

    // }

    

    // fileName dataFile (dataPath/"Uz_mode" + "1");

    // volScalarField Uz_mode
    // (
    //     IOobject
    //     (
    //         "Uz_mode1_data",
    //         dataPath,
    //         mesh,
    //         IOobject::NO_READ,
    //         IOobject::AUTO_WRITE
    //     ),
    //     mesh,
    //     dimVelocity
    // );

    // IFstream dataStream(dataFile);

    // forAll(mesh.cellZones()[0], cellI)
    // {
    //     forAll(mesh.cellZones(), zoneI)
    //     {
    //         label cell = mesh.cellZones()[zoneI][cellI];
    //         dataStream.read(Uz_mode[cell]);
    //     }
    // }

    // Uz_mode.write();
    
    // forAll(mesh.cellZones(), zoneI)
    // {
    //     Info << endl << "cellZone name: " << mesh.cellZones()[zoneI].name() << endl;

    //     outputFile_.width(16);
    //     outputFile_ << mesh.cellZones()[zoneI].name();

    //     forAll(mesh.cellZones()[zoneI], cellI)
    //     {
    //         // mesh.cellZones()[zoneI][cellI] give the global cell number in the mesh
    //         const label cell = mesh.cellZones()[zoneI][cellI];
    //         // outputData_[zoneI].append(field_[cell]);

    //         outputData_[zoneI].resize(mesh.cellZones()[zoneI].size());
    //         outputData_[zoneI][cellI] = field_[cell];
    //     }            
    // }

    // Rsh, fileIO
    // fileName dataPath (mesh.time().path()/"postProcessing");
    // fileName dataFile (dataPath/"Uz_phi");

    // IFstream dataStream(dataFile);
    
    // scalar testData;
    // word testLine;

    // dataStream.getLine(testLine);

    // const auto parsed = stringOps::split(testLine, " ");
  
    // wordList words(parsed.size());

    // label i = 0;
    // for (const auto& sub : parsed)
    // {
    //     // Could easily filter out '.' here too
    //     words[i] = sub.str();
    //     ++i;
    // }

    // // As a plain wordList

    // Info << "words" << words << endl;

    // dataStream.read(testData);
    // dataStream.read(testData);

    // Info << "The read line is: " << testLine << endl;
    // Info << "The read data is: " << testData << endl;


    return 0;
}


// ************************************************************************* //
