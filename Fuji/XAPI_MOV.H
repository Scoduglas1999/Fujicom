/*
 *  XAPI_MOV.h
 *  
 *    FUJIFILM X Series Digital Camera Shooting SDK Header file
 *
 *    Version 1.10.0.0
 *
 *  Copyright (C) 2014-2021 FUJIFILM Corporation.   
 *
 */

#ifndef __XAPI_MOV_H__
#define __XAPI_MOV_H__

/////////////////////////////////////////////////////////////////////////////////////////////
//  API Code
enum{
    // Release Control
	API_CODE_GetRecordingStatus			= 0x1114,
    API_CODE_CapRecordingStatus         = 0x1119,

    // Exposure control
	API_CODE_CapMovieShutterSpeed		= 0x1340,
	API_CODE_SetMovieShutterSpeed		= 0x1341,
	API_CODE_GetMovieShutterSpeed		= 0x1342,
	API_CODE_CapMovieExposureBias		= 0x1343,
	API_CODE_SetMovieExposureBias		= 0x1344,
	API_CODE_GetMovieExposureBias		= 0x1345,
	API_CODE_CapMovieSensitivity		= 0x1346,
	API_CODE_SetMovieSensitivity		= 0x1347,
	API_CODE_GetMovieSensitivity		= 0x1348,
	API_CODE_CapMovieAperture			= 0x1349,
	API_CODE_SetMovieAperture			= 0x134A,
	API_CODE_GetMovieAperture			= 0x134B,
	API_CODE_CapMovieDynamicRange		= 0x1360,
	API_CODE_SetMovieDynamicRange		= 0x1361,
	API_CODE_GetMovieDynamicRange		= 0x1362,
	API_CODE_CapMovieMeteringMode		= 0x1363,
	API_CODE_SetMovieMeteringMode		= 0x1364,
	API_CODE_GetMovieMeteringMode		= 0x1365,
	API_CODE_CapMovieWBMode				= 0x1366,
	API_CODE_SetMovieWBMode				= 0x1367,
	API_CODE_GetMovieWBMode				= 0x1368,
	API_CODE_CapMovieWBColorTemp		= 0x1369,
	API_CODE_SetMovieWBColorTemp		= 0x136A,
	API_CODE_GetMovieWBColorTemp		= 0x136B,
};


//
// Constants defines
//

// Recording Status 
#define XSDK_REC_STATUS_NOT_AVAILABLE       0x0001
#define XSDK_REC_STATUS_STANDBY             0x0002
#define XSDK_REC_STATUS_RECORDING           0x0004
#define XSDK_REC_STATUS_STORING             0x0008

// Movie Sensitivity
#define XSDK_SENSITIVITY_MOVIE_AUTO         -10


#ifndef _WIN32
#pragma export on
#endif

//
// Export API defines
//
#ifdef  __cplusplus
extern "C" {
#endif  // _cplusplus

// Release Control
XSDK_APIENTRY XSDK_CapRecordingStatus( XSDK_HANDLE hCamera, long* plNum, long* plStatus );
XSDK_APIENTRY XSDK_GetRecordingStatus( XSDK_HANDLE hCamera, long* plStatus );

// Exposure control
XSDK_APIENTRY XSDK_CapMovieShutterSpeed(XSDK_HANDLE hCamera, long* plNumShutterSpeed, long long* pllShutterSpeed);
XSDK_APIENTRY XSDK_SetMovieShutterSpeed(XSDK_HANDLE hCamera, long long llShutterSpeed);
XSDK_APIENTRY XSDK_GetMovieShutterSpeed(XSDK_HANDLE hCamera, long long* pllShutterSpeed);
XSDK_APIENTRY XSDK_CapMovieExposureBias(XSDK_HANDLE hCamera, long* plNumExposureBias, long* plExposureBias);
XSDK_APIENTRY XSDK_SetMovieExposureBias(XSDK_HANDLE hCamera, long lExposureBias);
XSDK_APIENTRY XSDK_GetMovieExposureBias(XSDK_HANDLE hCamera, long* plExposureBias);
XSDK_APIENTRY XSDK_CapMovieSensitivity(XSDK_HANDLE hCamera, long* plNumSensitivity, long* plSensitivity);
XSDK_APIENTRY XSDK_SetMovieSensitivity(XSDK_HANDLE hCamera, long lSensitivity);
XSDK_APIENTRY XSDK_GetMovieSensitivity(XSDK_HANDLE hCamera, long* plSensitivity);
XSDK_APIENTRY XSDK_CapMovieAperture(XSDK_HANDLE hCamera, long* plNumAperture, long* plFNumber);
XSDK_APIENTRY XSDK_SetMovieAperture(XSDK_HANDLE hCamera, long lFNumber);
XSDK_APIENTRY XSDK_GetMovieAperture(XSDK_HANDLE hCamera, long* plFNumber);
XSDK_APIENTRY XSDK_CapMovieDynamicRange( XSDK_HANDLE hCamera, long* plNumDynamicRange, long* plDynamicRange );
XSDK_APIENTRY XSDK_SetMovieDynamicRange( XSDK_HANDLE hCamera, long lDynamicRange );
XSDK_APIENTRY XSDK_GetMovieDynamicRange( XSDK_HANDLE hCamera, long* plDynamicRange );
XSDK_APIENTRY XSDK_CapMovieMeteringMode( XSDK_HANDLE hCamera, long* plNumMeteringMode, long* plMeteringMode );
XSDK_APIENTRY XSDK_SetMovieMeteringMode( XSDK_HANDLE hCamera, long lMeteringMode );
XSDK_APIENTRY XSDK_GetMovieMeteringMode( XSDK_HANDLE hCamera, long* plMeteringMode );
XSDK_APIENTRY XSDK_CapMovieWBMode( XSDK_HANDLE hCamera, long* plNumWBMode, long* plWBMode );
XSDK_APIENTRY XSDK_SetMovieWBMode( XSDK_HANDLE hCamera, long lWBMode );
XSDK_APIENTRY XSDK_GetMovieWBMode( XSDK_HANDLE hCamera, long* plWBMode );
XSDK_APIENTRY XSDK_CapMovieWBColorTemp( XSDK_HANDLE hCamera, long* plNumWBColorTemp, long* plWBColorTemp );
XSDK_APIENTRY XSDK_SetMovieWBColorTemp( XSDK_HANDLE hCamera, long lColorTemp );
XSDK_APIENTRY XSDK_GetMovieWBColorTemp( XSDK_HANDLE hCamera, long* plColorTemp );

#ifdef  __cplusplus
}
#endif  // _cplusplus

#ifndef _WIN32
#pragma export off
#endif

//
// Import function types define
//

// Release Control
typedef XSDK_APIENTRY  (*_XSDK_CapRecordingStatus)(XSDK_HANDLE hCamera, long* plNum, long* plStatus);
typedef XSDK_APIENTRY  (*_XSDK_GetRecordingStatus)(XSDK_HANDLE hCamera, long* plStatus);

// Exposure control
typedef XSDK_APIENTRY (*_XSDK_CapMovieShutterSpeed)(XSDK_HANDLE hCamera, long* plNumShutterSpeed, long long* pllShutterSpeed);
typedef XSDK_APIENTRY (*_XSDK_SetMovieShutterSpeed)(XSDK_HANDLE hCamera, long long llShutterSpeed);
typedef XSDK_APIENTRY (*_XSDK_GetMovieShutterSpeed)(XSDK_HANDLE hCamera, long long* pllShutterSpeed);
typedef XSDK_APIENTRY (*_XSDK_CapMovieExposureBias)(XSDK_HANDLE hCamera, long* plNumExposureBias, long* plExposureBias);
typedef XSDK_APIENTRY (*_XSDK_SetMovieExposureBias)(XSDK_HANDLE hCamera, long lExposureBias);
typedef XSDK_APIENTRY (*_XSDK_GetMovieExposureBias)(XSDK_HANDLE hCamera, long* plExposureBias);
typedef XSDK_APIENTRY (*_XSDK_CapMovieSensitivity)(XSDK_HANDLE hCamera, long* plNumSensitivity, long* plSensitivity);
typedef XSDK_APIENTRY (*_XSDK_SetMovieSensitivity)(XSDK_HANDLE hCamera, long lSensitivity);
typedef XSDK_APIENTRY (*_XSDK_GetMovieSensitivity)(XSDK_HANDLE hCamera, long* plSensitivity);
typedef XSDK_APIENTRY (*_XSDK_CapMovieAperture)(XSDK_HANDLE hCamera, long* plNumAperture, long* plFNumber);
typedef XSDK_APIENTRY (*_XSDK_SetMovieAperture)(XSDK_HANDLE hCamera, long lFNumber);
typedef XSDK_APIENTRY (*_XSDK_GetMovieAperture)(XSDK_HANDLE hCamera, long* plFNumber);
typedef XSDK_APIENTRY  (*_XSDK_CapMovieDynamicRange)(XSDK_HANDLE hCamera, long* plNumDynamicRange, long* plDynamicRange );
typedef XSDK_APIENTRY  (*_XSDK_SetMovieDynamicRange)(XSDK_HANDLE hCamera, long lDynamicRange );
typedef XSDK_APIENTRY  (*_XSDK_GetMovieDynamicRange)(XSDK_HANDLE hCamera, long* plDynamicRange );
typedef XSDK_APIENTRY  (*_XSDK_CapMovieMeteringMode)(XSDK_HANDLE hCamera, long* plNumMeteringMode, long* plMeteringMode );
typedef XSDK_APIENTRY  (*_XSDK_SetMovieMeteringMode)(XSDK_HANDLE hCamera, long lMeteringMode );
typedef XSDK_APIENTRY  (*_XSDK_GetMovieMeteringMode)(XSDK_HANDLE hCamera, long* plMeteringMode );
typedef XSDK_APIENTRY  (*_XSDK_CapMovieWBMode)(XSDK_HANDLE hCamera, long* plNumWBMode, long* plWBMode );
typedef XSDK_APIENTRY  (*_XSDK_SetMovieWBMode)(XSDK_HANDLE hCamera, long lWBMode );
typedef XSDK_APIENTRY  (*_XSDK_GetMovieWBMode)(XSDK_HANDLE hCamera, long* plWBMode );
typedef XSDK_APIENTRY  (*_XSDK_CapMovieWBColorTemp)(XSDK_HANDLE hCamera, long* plNumWBColorTemp, long* plWBColorTemp );
typedef XSDK_APIENTRY  (*_XSDK_SetMovieWBColorTemp)(XSDK_HANDLE hCamera, long lColorTemp );
typedef XSDK_APIENTRY  (*_XSDK_GetMovieWBColorTemp)(XSDK_HANDLE hCamera, long* plColorTemp );

#endif  // __XAPI_MOV_H__