/*  File    : ardupilot_sil_connector.cpp
 *  Abstract:
 *
 *  Simulink C++ S-function for software-in-the-loop (SIL) simulation with ArduPilot.
 *
 *  Copyright (c) 2022 Kiril Boychev
 */

#include <sstream>
#include <string>

#include <asio.hpp>

#define S_FUNCTION_LEVEL 2
//#define S_FUNCTION_DEBUG
#define S_FUNCTION_NAME ardupilot_sil_connector

/*
 * Need to include simstruc.h for the definition of the SimStruct and
 * its associated macro definitions.
 */
#include "simstruc.h"

/*====================*
 * S-function methods *
 *====================*/

/* Function: mdlInitializeSizes ===============================================
 * Abstract:
 *    The sizes information is used by Simulink to determine the S-function
 *    block's characteristics (number of inputs, outputs, states, etc.).
 */
static void mdlInitializeSizes(SimStruct *S)
{
    ssSetNumSFcnParams(S, 0); /* Number of expected parameters */

    if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S))
    {
        return;
    }

    if (!ssSetNumInputPorts(S, 6)) /* Number of input ports */
    {
        return;
    }

    /* Input ports size */
    ssSetInputPortWidth(S, 0, 1); // time (s)
    ssSetInputPortDirectFeedThrough(S, 0, 1);
    ssSetInputPortWidth(S, 1, 3); // omega_b (rad/s)
    ssSetInputPortDirectFeedThrough(S, 1, 1);
    ssSetInputPortWidth(S, 2, 3); // A (m/s^2)
    ssSetInputPortDirectFeedThrough(S, 2, 1);
    ssSetInputPortWidth(S, 3, 3); // xyzned (m)
    ssSetInputPortDirectFeedThrough(S, 3, 1);
    ssSetInputPortWidth(S, 4, 3); // phithetapsi (rad)
    ssSetInputPortDirectFeedThrough(S, 4, 1);
    ssSetInputPortWidth(S, 5, 3); // Vbned (m/s)
    ssSetInputPortDirectFeedThrough(S, 5, 1);

    if (!ssSetNumOutputPorts(S, 1)) /* Number of output ports */
    {
        return;
    }

    /* Output ports size */
    ssSetOutputPortWidth(S, 0, 16);

    /* Number of sample times */
    ssSetNumSampleTimes(S, 1);

    /* Number of PWork vector */
    ssSetNumPWork(S, 2);

    ssSetOptions(S, 0);
}

/* Function: mdlInitializeSampleTimes =========================================
 * Abstract:
 *    This function is used to specify the sample time(s) for your
 *    S-function. You must register the same number of sample times as
 *    specified in ssSetNumSampleTimes.
 */
static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S, 0, INHERITED_SAMPLE_TIME);
    ssSetOffsetTime(S, 0, 0.0);
    ssSetModelReferenceSampleTimeDefaultInheritance(S);
}

#define MDL_START /* Change to #undef to remove function */
#if defined(MDL_START)
/* Function: mdlStart =======================================================
 * Abstract:
 *    This function is called once at start of model execution. If you
 *    have states that should be initialized once, this is the place
 *    to do it.
 */
static void mdlStart(SimStruct *S)
{
    try
    {
        void **PWork = ssGetPWork(S);

#ifndef S_FUNCTION_DEBUG
        static asio::io_service io_service;
        asio::ip::udp::endpoint endpoint(asio::ip::address::from_string("0.0.0.0"), 9002);
        static asio::ip::udp::socket socket(io_service, endpoint.protocol());
        if (!socket.is_open())
        {
            socket.open(endpoint.protocol());
        }
        socket.set_option(asio::socket_base::reuse_address(true));
        socket.set_option(asio::detail::socket_option::integer<SOL_SOCKET, SO_RCVTIMEO>{2000});
        static double pwm[16];
        for (unsigned int i = 0; i < 16; i++)
        {
            pwm[i] = 0.0;
        }
        PWork[0] = &socket;
        PWork[1] = &pwm;

        socket.bind(endpoint);
#endif
    }
    catch (const asio::system_error &e)
    {
        static char errorStatus[256];
        sprintf(errorStatus, "%s\n", e.what());
        ssSetErrorStatus(S, errorStatus);
    }
}
#endif /*  MDL_START */

/* Function: mdlOutputs =======================================================
 * Abstract:
 *    In this function, you compute the outputs of your S-function
 *    block.
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
    try
    {
        void **PWork = ssGetPWork(S);

        asio::ip::udp::socket *socket = (asio::ip::udp::socket *)PWork[0];

        double *pwm = ((double *)PWork[1]);

        InputRealPtrsType time = ssGetInputPortRealSignalPtrs(S, 0);
        InputRealPtrsType omega_b = ssGetInputPortRealSignalPtrs(S, 1);
        InputRealPtrsType A = ssGetInputPortRealSignalPtrs(S, 2);
        InputRealPtrsType xyzned = ssGetInputPortRealSignalPtrs(S, 3);
        InputRealPtrsType phithetapsi = ssGetInputPortRealSignalPtrs(S, 4);
        InputRealPtrsType Vbned = ssGetInputPortRealSignalPtrs(S, 5);

        real_T *pwm_out = ssGetOutputPortRealSignal(S, 0);

#ifndef S_FUNCTION_DEBUG
        const auto availableLength = socket->available();

        if (availableLength)
        {
            uint8_t buffer[256];
            asio::ip::udp::endpoint endpoint;

            socket->receive_from(asio::buffer(buffer, availableLength), endpoint);

            for (uint8_t i = 0; i < 16; i++)
            {
                pwm[i] = (real_T)(((uint16_t)buffer[2 * i + 1 + 8] << 8) | buffer[2 * i + 8]);
            }

            std::stringstream ss;

            ss << "\n{\"timestamp\":" << (*time[0]) << ",";
            ss << "\"imu\":";
            ss << "{";
            ss << "\"gyro\":[" << (*omega_b[0]) << "," << (*omega_b[1]) << "," << (*omega_b[2]) << "],";
            ss << "\"accel_body\":[" << (*A[0]) << "," << (*A[1]) << "," << (*A[2]) << "]},";
            ss << "\"position\":[" << (*xyzned[0]) << "," << (*xyzned[1]) << "," << (*xyzned[2]) << "],";
            ss << "\"attitude\":[" << (*phithetapsi[0]) << "," << (*phithetapsi[1]) << "," << (*phithetapsi[2]) << "],";
            ss << "\"velocity\":[" << (*Vbned[0]) << "," << (*Vbned[1]) << "," << (*Vbned[2]) << "]";
            ss << "}\n";

            const auto s = ss.str();

            socket->send_to(asio::buffer(s), endpoint);
        }
#endif
        for (unsigned int i = 0; i < 16; i++)
        {
            pwm_out[i] = pwm[i];
        }
    }
    catch (const asio::system_error &e)
    {
        static char errorStatus[256];
        sprintf(errorStatus, "%s\n", e.what());
        ssSetErrorStatus(S, errorStatus);
    }
}

/* Function: mdlTerminate =====================================================
 * Abstract:
 *    In this function, you should perform any actions that are necessary
 *    at the termination of a simulation.  For example, if memory was
 *    allocated in mdlStart, this is the place to free it.
 */
static void mdlTerminate(SimStruct *S)
{
    try
    {
        void **PWork = ssGetPWork(S);
        if (PWork[0] != nullptr)
        {
            ((asio::ip::udp::socket *)PWork[0])->close();
        }
    }
    catch (const asio::system_error &e)
    {
        static char errorStatus[256];
        sprintf(errorStatus, "%s\n", e.what());
        ssSetErrorStatus(S, errorStatus);
    }
}
/*======================================================*
 * See sfuntmpl.doc for the optional S-function methods *
 *======================================================*/

/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef MATLAB_MEX_FILE /* Is this file being compiled as a MEX-file? */
#include "simulink.c"  /* MEX-file interface mechanism */
#else
#include "cg_sfun.h" /* Code generation registration function */
#endif
