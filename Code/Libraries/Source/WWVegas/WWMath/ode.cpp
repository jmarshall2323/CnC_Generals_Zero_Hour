/*
**	Command & Conquer Generals(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* $Header: /Commando/Code/wwmath/ODE.CPP 8     7/02/99 10:32a Greg_h $ */
/*********************************************************************************************** 
 ***                            Confidential - Westwood Studios                              *** 
 *********************************************************************************************** 
 *                                                                                             * 
 *                 Project Name : Commando                                                     * 
 *                                                                                             * 
 *                     $Archive:: /Commando/Code/wwmath/ODE.CPP                               $* 
 *                                                                                             * 
 *                       Author:: Greg_h                                                       * 
 *                                                                                             * 
 *                     $Modtime:: 6/25/99 6:23p                                               $* 
 *                                                                                             * 
 *                    $Revision:: 8                                                           $* 
 *                                                                                             * 
 *---------------------------------------------------------------------------------------------* 
 * Functions:                                                                                  * 
 *   Euler_Integrate -- uses Eulers method to integrate a system of ODE's                      * 
 *   Midpoint_Integrate -- midpoint method (Runge-Kutta 2) for integration                     * 
 *   Runge_Kutta_Integrate -- Runge Kutta 4 method                                             * 
 *   Runge_Kutta5_Integrate -- 5th order Runge-Kutta                                           * 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "ode.h"
#include <assert.h>

static std::vector<float> Y0;
static std::vector<float> Y1;
static std::vector<float> _WorkVector0;
static std::vector<float> _WorkVector1;
static std::vector<float> _WorkVector2;
static std::vector<float> _WorkVector3;
static std::vector<float> _WorkVector4;
static std::vector<float> _WorkVector5;
static std::vector<float> _WorkVector6;
static std::vector<float> _WorkVector7;

/*********************************************************************************************** 
 * Euler_Solve -- uses Eulers method to integrate a system of ODE's                            * 
 *                                                                                             * 
 * INPUT:                                                                                      * 
 * odesys - pointer to the ODE system to integrate                                             * 
 * dt - size of the timestep                                                                   * 
 *                                                                                             * 
 * OUTPUT:                                                                                     * 
 * state vector in odesys will be updated for the next timestep                                * 
 *                                                                                             * 
 * WARNINGS:                                                                                   * 
 *                                                                                             * 
 * HISTORY:                                                                                    * 
 *   08/11/1997 GH  : Created.                                                                 * 
 *   6/25/99    GTH : Updated to the new integrator system                                     *
 *=============================================================================================*/
void IntegrationSystem::Euler_Integrate(ODESystemClass * sys, float dt)
{
	WWASSERT(sys != NULL);

	/*
	** Get the current state
	*/
	Y0.clear();
	sys->Get_State(Y0);

	/*
	** make aliases to the work-vectors we need
	*/
	std::vector<float>& dydt = _WorkVector0;
	dydt.clear();
	dydt.reserve(Y0.size());

	/*
	** Euler method, just evaluate the derivative, multiply
	** by the time-step and add to the current state vector.
	*/
	sys->Compute_Derivatives(0,NULL,&dydt);

	Y1.clear();
	Y1.reserve(Y0.size());
	for (size_t i = 0; i < Y0.size(); i++)
	{
		const float result = Y0[i] + dydt[i] * dt;
		Y1.push_back(result);
	}

	sys->Set_State(Y1);
}

/*********************************************************************************************** 
 * Midpoint_Integrate -- midpoint method (Runge-Kutta 2)                                       * 
 *                                                                                             * 
 * INPUT:                                                                                      * 
 * sys - pointer to the ODE system to integrate                                                * 
 * dt - size of the timestep                                                                   * 
 *                                                                                             * 
 * OUTPUT:                                                                                     * 
 * state vector in odesys will be updated for the next timestep                                * 
 *                                                                                             * 
 * WARNINGS:                                                                                   * 
 *                                                                                             * 
 * HISTORY:                                                                                    * 
 *   08/11/1997 GH  : Created.                                                                 * 
 *   6/25/99    GTH : Updated to the new integrator system                                     *
 *=============================================================================================*/
void IntegrationSystem::Midpoint_Integrate(ODESystemClass * sys,float dt)
{
	/*
	** Get the current state
	*/
	Y0.clear();
	sys->Get_State(Y0);

	/*
	** make aliases to the work-vectors we need
	*/
	std::vector<float>& dydt = _WorkVector0;
	std::vector<float>& ymid = _WorkVector1;
	dydt.clear();
	dydt.reserve(Y0.size());
	ymid.clear();
	ymid.reserve(Y0.size());

	/*
	** MidPoint method, first evaluate the derivitives of the
	** state vector just like the Euler method.  
	*/
	sys->Compute_Derivatives(0.0f,NULL,&dydt);

	/*
	** Compute the midpoint between the Euler solution and 
	** the input values.
	*/
	for (size_t i = 0; i < Y0.size(); i++)
	{
		const float result = Y0[i] + dt * dydt[i] / 2.0f;
		ymid.push_back(result);
	}
	
	/*
	** Re-compute derivatives at this point.  
	*/
	sys->Compute_Derivatives(dt/2.0f,&ymid,&dydt);

	/*
	** Use these derivatives to compute the solution.
	*/
	Y1.clear();
	Y1.reserve(Y0.size());
	for (size_t i = 0; i < Y0.size(); i++)
	{
		const float result = Y0[i] + dt * dydt[i];
		Y1.push_back(result);
	}

	sys->Set_State(Y1);
}


/*********************************************************************************************** 
 * Runge_Kutta_Integrate -- Runge Kutta 4 method                                               * 
 *                                                                                             * 
 * INPUT:                                                                                      * 
 * odesys - pointer to the ODE system to integrate                                             * 
 * dt - size of the timestep                                                                   * 
 *                                                                                             * 
 * OUTPUT:                                                                                     * 
 * state vector in odesys will be updated for the next timestep                                * 
 *                                                                                             * 
 * WARNINGS:                                                                                   * 
 *                                                                                             * 
 * HISTORY:                                                                                    * 
 *   08/11/1997 GH  : Created.                                                                 * 
 *=============================================================================================*/
void IntegrationSystem::Runge_Kutta_Integrate(ODESystemClass * sys,float dt)
{
	float dt2 = dt / 2.0f;
	float dt6 = dt / 6.0f;

	/*
	** Get the current state
	*/
	Y0.clear();
	sys->Get_State(Y0);

	/*
	** make aliases to the work-vectors we need
	*/
	std::vector<float>& dydt = _WorkVector0;
	std::vector<float>& dym  = _WorkVector1;
	std::vector<float>& dyt  = _WorkVector2;
	std::vector<float>& yt   = _WorkVector3;
	dydt.clear();
	dydt.reserve(Y0.size());
	dym.clear();
	dym.reserve(Y0.size());
	dyt.clear();
	dyt.reserve(Y0.size());
	yt.clear();
	yt.reserve(Y0.size());

	/*
	** First Step
	*/
	sys->Compute_Derivatives(0.0f,NULL,&dydt);
	for (size_t i = 0; i < Y0.size(); i++)
	{
		const float result = Y0[i] + dt2 * dydt[i];
		yt.push_back(result);
	}
	
	/*
	** Second Step
	*/
	sys->Compute_Derivatives(dt2, &yt, &dyt);
	for (size_t i = 0; i < Y0.size(); i++)
	{
		yt[i] = Y0[i] + dt2 * dyt[i];
	}
	
	/*
	** Third Step
	*/
	sys->Compute_Derivatives(dt2, &yt, &dym);
	for (size_t i = 0; i < Y0.size(); i++)
	{
		yt[i] = Y0[i] + dt*dym[i];
		dym[i] += dyt[i];
	}

	/*
	** Fourth Step
	*/
	sys->Compute_Derivatives(dt, &yt, &dyt);

	Y1.clear();
	Y1.reserve(Y0.size());
	for (size_t i = 0; i < Y0.size(); i++)
	{
		Y1[i] = Y0[i] + dt6 * (dydt[i] + dyt[i] + 2.0f*dym[i]);
	}

	sys->Set_State(Y1);
}

/*********************************************************************************************** 
 * Runge_Kutta5_Integrate -- 5th order Runge-Kutta                                             * 
 *                                                                                             * 
 * INPUT:                                                                                      * 
 * odesys - pointer to the ODE system to integrate                                             * 
 * dt - size of the timestep                                                                   * 
 *                                                                                             * 
 * OUTPUT:                                                                                     * 
 * state vector in odesys will be updated for the next timestep                                * 
 *                                                                                             * 
 * WARNINGS:                                                                                   * 
 *                                                                                             * 
 * HISTORY:                                                                                    * 
 *   08/11/1997 GH  : Created.                                                                 * 
 *   6/25/99    GTH : Converted to the new Integrator system                                   *
 *=============================================================================================*/
void IntegrationSystem::Runge_Kutta5_Integrate(ODESystemClass * odesys,float dt)
{
	static const float a2 =			0.2f;
	static const float a3 =			0.3f;
	static const float a4 =			0.6f;
	static const float a5 =			1.0f;
	static const float a6 =			0.875f;
	static const float b21 =		0.2f;
	static const float b31 =		3.0f/40.0f;
	static const float b32 =		9.0f/40.0f;
	static const float b41 =		0.3f;
	static const float b42 =		-0.9f;
	static const float b43 =		1.2f;
	static const float b51 =		-11.0f /54.0f;
	static const float b52 =		2.5f;
	static const float b53 =		-70.0f/27.0f;
	static const float b54 =		35.0f/27.0f;
	static const float b61 =		1631.0f/55296.0f;
	static const float b62 =		175.0f/512.0f;
	static const float b63 =		575.0f/13824.0f;
	static const float b64 =		44275.0f/110592.0f;
	static const float b65 =		253.0f/4096.0f;
	static const float c1 =			37.0f/378.0f;
	static const float c3 =			250.0f/621.0f;
	static const float c4 =			125.0f/594.0f;
	static const float c6 =			512.0f/1771.0f;
	static const float dc5 =		-277.0f/14336.0f;
	static const float dc1 =		c1 - 2825.0f/27648.0f;
	static const float dc3 =		c3 - 18575.0f/48384.0f;
	static const float dc4 =		c4 - 13525.0f/55296.0f;
	static const float dc6 =		c6 - 0.25f;

	/*
	** Get the current state
	*/
	Y0.clear();
	odesys->Get_State(Y0);
	const size_t veclen = Y0.size();

	/*
	** make aliases to the work-vectors we need
	*/
	std::vector<float>& dydt	= _WorkVector0;
	std::vector<float>& ak2	= _WorkVector1;
	std::vector<float>& ak3	= _WorkVector2;
	std::vector<float>& ak4	= _WorkVector3;
	std::vector<float>& ak5	= _WorkVector4;
	std::vector<float>& ak6	= _WorkVector5;
	std::vector<float>& ytmp	= _WorkVector6;
	std::vector<float>& yerr	= _WorkVector7;

	dydt.clear();
	dydt.reserve(veclen);
	ak2.clear();
	ak2.reserve(veclen);
	ak3.clear();
	ak3.reserve(veclen);
	ak4.clear();
	ak4.reserve(veclen);
	ak5.clear();
	ak5.reserve(veclen);
	ak6.clear();
	ak6.reserve(veclen);
	ytmp.clear();
	ytmp.reserve(veclen);
	yerr.clear();
	yerr.reserve(veclen);

	// First step
	odesys->Compute_Derivatives(0.0f,NULL,&dydt);
	for (size_t i = 0; i < veclen; i++)
	{
		const float result = Y0[i] + b21*dt*dydt[i];
		ytmp.push_back(result);
	}

	// Second step
	odesys->Compute_Derivatives(a2*dt, &ytmp, &ak2);
	ytmp.clear();
	for (size_t i = 0; i < veclen; i++)
	{
		const float result = Y0[i] + dt*(b31*dydt[i] + b32*ak2[i]);
		ytmp.push_back(result);
	}
	
	// Third step
	odesys->Compute_Derivatives(a3*dt, &ytmp, &ak3);
	ytmp.clear();
	for (size_t i = 0; i < veclen; i++)
	{
		const float result = Y0[i] + dt*(b41*dydt[i] + b42*ak2[i] + b43*ak3[i]);
		ytmp.push_back(result);
	}
	
	// Fourth step
	odesys->Compute_Derivatives(a4*dt, &ytmp, &ak4);
	ytmp.clear();
	for (size_t i = 0; i < veclen; i++)
	{
		const float result = Y0[i] + dt*(b51*dydt[i] + b52*ak2[i] + b53*ak3[i] + b54*ak4[i]);
		ytmp.push_back(result);
	}

	// Fifth step
	odesys->Compute_Derivatives(a5*dt, &ytmp, &ak5);
	ytmp.clear();
	for (size_t i = 0; i < veclen; i++)
	{
		const float result = Y0[i] + dt*(b61*dydt[i] + b62*ak2[i] + b63*ak3[i] + b64*ak4[i] + b65*ak5[i]);
		ytmp.push_back(result);
	}

	// Sixth step
	odesys->Compute_Derivatives(a6*dt, &ytmp, &ak6);

	Y1.clear();
	Y1.reserve(veclen);
	for (size_t i = 0; i < veclen; i++)
	{
		const float result = Y0[i] + dt*(c1*dydt[i] + c3*ak3[i] + c4*ak4[i] + c6*ak6[i]);
		Y1.push_back(result);
	}

	// Error approximation!  
	// (maybe I should use this someday? nah not going to use this integrator anyway...)
	yerr.clear();
	for (size_t i = 0; i < veclen; i++)
	{
		const float result = dt*(dc1*dydt[i] + dc3*ak3[i] + dc4*ak4[i] + dc5*ak5[i] + dc6*ak6[i]);
		yerr.push_back(result);
	}

	odesys->Set_State(Y1);
}

