Runspace Sample 06
==========================
     This sample introduces how to create an InitialSessionState
     and then use InitialSessionState to create the Runspace.

     This sample assumes that user has the binary module "GetProcessSample02.dll" 
     produced in sample GetProcessSample01 copied to the current directory.

Sample Language Implementations
===============================
     This sample is available in the following language implementations:

     - C#

To build the sample using Visual Studio:
=======================================
     1. Open Windows Explorer and navigate to Runspace06 under the samples directory.
     2. Double-click the icon for the .sln (solution) file to open the file in Visual Studio.
     3. In the Build menu, select Build Solution.
     The library will be built in the default \bin or \bin\Debug directory.

To run the sample:
=================
     1. Build the sample GetProcessSample02.
     2. Start command prompt.
     3. Navigate to the folder containing the sample executable.
     4. Copy GetProcessSample02.dll to the current directory.
     5. Run the executable

Demonstrates
============
     This sample demonstrates the following:

     1. Creating a default InitialSessionState object.
     2. Creating a runspace from that InitialSessionState object.
     3. Creating a PowerShell object
     4. Creating a command using a module-qualified name and adding
        it to the pipeline.
     5. Woking with the PSObjects returned from the invocation.

