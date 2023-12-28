# brix ( tips & tricks )
Code bricks for faster application development.<br><br>
This Code has MIT License, but any help is welcome.<br>
To help improve the code quality you can:
- Send a minimal, reproducible example that produces the same problem.
- Send the solution of the problem.
- Both.
You can contact me on social media ( Discord / X ).

## looprecord
- I have originally developed this algorith for the Amiga(m68k assembly) for audio recording. It could record the last few seconds of samples, until the mouse button was pressed. I lost the code so I needed to rewrite it in c++.
- It has several use cases like: debug logs, catching only the last few records of data for various purposes.
- It has a small memory and cpu footprint, so it can be used on week / old devices.

## corestorage
- std storage classes with extended functions.
- coremodset & coremodmap for example has a modificator function which helps to modify the map itself in a range based loop( c++11 ). For usage example see test#2.

## corestring
- std::string with extended functionality.

## rng / hash / shuffler
- simple random number generator( class, single, function ).
- multipurpose / flexible hash algorithm based on rng class
- shuffler - mixing bytes order in array form with rng algorithm

## jsonparser
- simple, efficient json codec. I have tested a lot, but because the algorithm is very complex it is not recommended to use safety critical projects.
---
# Practical use cases
- Looprecord could help autonomous cars to record the last minute or a few seconds of input data before an accident ( it could also run in the background ).
1. Keeping all relevant raw data in memory( limited by looprecord).
2. Right after the event it will be packed into structured single package( pictures -> video, measurement -> compressed csv / sqlite / raw data / other practical form, etc. ).
3. The files should be saved into a non-volatile storage with timestamps ( + event type ).
  The event trigger could be also a problematic situations ( the software runs into an unexpected state ) or a user request.
