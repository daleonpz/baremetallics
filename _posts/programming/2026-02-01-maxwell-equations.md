---
layout: post
title: Maxwell's Equations
categories: programming
description:
tags: [pcb, embedded, physics]
---

* 
{:toc}


# 1. Gauss’s Law

Describen la influencie de una carga en el campo electrico que la rodia.

## Differential Form

describe exactamente la influencia de una sola carga en el campo electrico que la rodea.

Que tanto una carga hace que el $\mathbf{E}$ campo electrico se "diverge" alrededor de ella.
Como la gravedad, si divergencia es **negativa**, o antigravedad, si divergencia es **positiva**.

$$
\nabla \cdot \mathbf{E} = \frac{\rho}{\varepsilon_0}
$$

| Symbol | Meaning | SI Unit |
|---|---|---|
| $\mathbf{E}$ | Electric field | $\mathrm{V/m}$ |
| $\rho$ | Charge density | $\mathrm{C/m^3}$ |
| $\varepsilon_0$ | Vacuum permittivity | $\mathrm{F/m}$ |


<!-- add a picture of a charge with electric field lines diverging from it -->


## Integral Form
 describa cuanta "energia" o "potencial" hay debido a la presencia de una carga.
Describes how much force (potential) a charge exerts on other charges in its vicinity.
It states that the total electric flux through a closed surface is proportional to the charge enclosed by that surface.

$$
\oint_S \mathbf{E} \cdot d\mathbf{A}
=
\frac{Q_{\text{enc}}}{\varepsilon_0}
$$

| Symbol | Meaning | SI Unit |
|---|---|---|
| $Q_{\text{enc}}$ | Enclosed charge | $\mathrm{C}$ |
| $d\mathbf{A}$ | Differential area vector | $\mathrm{m^2}$ |


## How Electric Fields are related to Currents

Given a current density $\mathbf{J}$, we can relate it to the electric field $\mathbf{E}$:

$$
\mathbf{J} = \sigma \mathbf{E}
$$

where $\sigma$ is the electrical conductivity of the material. This tells us how easily charges can move through the material in response to an electric field. In a conductor, $\sigma$ is high, so a small electric field can produce a large current density. In an insulator, $\sigma$ is low, so a larger electric field is needed to produce the same current density.

$$
I = \int_S \mathbf{J} \cdot d\mathbf{A}
$$

then 

$$
I = \int_S \sigma \mathbf{E} \cdot d\mathbf{A}
$$

if we assume an uniform cross-sectional area $A$ and a uniform electric field $\mathbf{E}$ along the length of the wire, this simplifies to:

$$
I = \sigma E A
$$


Electric potential $V$ in a wire is related to the electric field $\mathbf{E}$ by:

$$
V_{ab} = -\int_a^b \mathbf{E} \cdot d\mathbf{l}
$$

if we assume a uniform electric field along the length of the wire, this simplifies to:

$$
V = E \cdot L
$$

where $E$ becomes:

$$
E = \frac{V}{L}
$$

then replacing $E$ in the equation for current $I$:

$$
I = \sigma E A
$$


$$
I = \sigma \frac{V}{L} A
$$

and Resistance $R$ is defined as:

$$
R = \frac{V}{I} = \frac{L}{\sigma A}
$$


---

# 2. Gauss’s Law for Magnetism

Describen el comportamiento de los campos magneticos.

## Differential Form

dice que los campos magneticos $\mathbf{B}$ forman siempre loops que van de norte a sur.

$$
\oint_S \mathbf{B} \cdot d\mathbf{A} = 0
$$

| Symbol | Meaning | SI Unit |
|---|---|---|
| $\mathbf{B}$ | Magnetic flux density | $\mathrm{T}$ |
| $d\mathbf{A}$ | Differential area vector | $\mathrm{m^2}$ |

## Integral Form

dice que si se coloca una carga magnetica $q_m$ en un campo magnetico $\mathbf{B}$, la carga $q_m$ no va a poder solo "alejar" o "atraer" el campo magnetico $\mathbf{B}$, ya que no existen los monopolos. Esa carga $q_m$ siempre va a tener un norte y un sur, el norte se "aleja" y el sur "atrae", lo que da como resultado un flujo neto de campo magnetico $\mathbf{B}$ a traves de cualquier superficie cerrada igual a cero. A differencia de las cargas electricas que si pueden solo "alejar" o "atraer" el campo electrico $\mathbf{E}$ alrededor de ellas, lo que da como resultado un flujo neto de campo electrico $\mathbf{E}$ a traves de cualquier superficie cerrada diferente de cero.

$$
\nabla \cdot \mathbf{B} = 0
$$

| Symbol | Meaning | SI Unit |
|---|---|---|
| $\mathbf{B}$ | Magnetic flux density | $\mathrm{T}$ (tesla) |

---

# 3. Faraday’s Law of Induction

Describe la relacion como campos magneticos pueden generar campos electricos.

## Differential Form

describe que un cambio en un campo magnetico $\partial \mathbf{B}/\partial t$ induce un campo electrico $\mathbf{E}$. Y te dice que tan facil se puede hacer girar ese campo electrico $\mathbf{E}$ alrededor de un punto, o inidicates the circutaltion of a vector field around a point.

$$
\nabla \times \mathbf{E}
=
-\frac{\partial \mathbf{B}}{\partial t}
$$

| Symbol | Meaning | SI Unit |
|---|---|---|
| $\mathbf{E}$ | Electric field | $\mathrm{V/m}$ |
| $\mathbf{B}$ | Magnetic flux density | $\mathrm{T}$ |


## Integral Form

describe que un cambio en el flujo magnetico $\int_S \mathbf{B} \cdot d\mathbf{A}$ a traves de una superficie $S$ induce una fuerza electromotriz (emf) o voltaje $V$ en un circuito cerrado $C$ que rodea esa superficie $S$.

Eso quiere decir que si se connecta una resistencia a ese circuito cerrado $C$, se va a generar una corriente $I$ en ese circuito cerrado $C$ debido a la fuerza electromotriz (emf) inducida.

$$
\oint_C \mathbf{E} \cdot d\mathbf{l}
=
-
\frac{d}{dt}
\int_S \mathbf{B} \cdot d\mathbf{A}
$$

| Symbol | Meaning | SI Unit |
|---|---|---|
| $d\mathbf{l}$ | Differential line element | $\mathrm{m}$ |
| $\mathbf{B}\cdot d\mathbf{A}$ | Magnetic flux element | $\mathrm{Wb}$ |

---

# 4. Ampère–Maxwell Law

Describe la relacion como campos electricos pueden generar campos magneticos.

## Differential Form

$$
\nabla \times \mathbf{B}
=
\mu_0 \mathbf{J}
+
\mu_0 \varepsilon_0
\frac{\partial \mathbf{E}}{\partial t}
$$

| Symbol | Meaning | SI Unit |
|---|---|---|
| $\mathbf{B}$ | Magnetic flux density | $\mathrm{T}$ |
| $\mathbf{J}$ | Current density | $\mathrm{A/m^2}$ |
| $\mu_0$ | Vacuum permeability | $\mathrm{H/m}$ |
| $\varepsilon_0$ | Vacuum permittivity | $\mathrm{F/m}$ |
| $\mathbf{E}$ | Electric field | $\mathrm{V/m}$ |

## Integral Form


$$
\oint_C \mathbf{B} \cdot d\mathbf{l}
=
\mu_0 I_{\text{enc}}
+
\mu_0 \varepsilon_0
\frac{d}{dt}
\int_S \mathbf{E} \cdot d\mathbf{A}
$$

| Symbol | Meaning | SI Unit |
|---|---|---|
| $I_{\text{enc}}$ | Enclosed current | $\mathrm{A}$ |
| $\mathbf{E}\cdot d\mathbf{A}$ | Electric flux element | $\mathrm{V \cdot m}$ |

---

# Useful Physical Constants (SI)

$$
\varepsilon_0 \approx 8.854 \times 10^{-12}\ \mathrm{F/m}
$$

$$
\mu_0 = 4\pi \times 10^{-7}\ \mathrm{H/m}
$$

$\varepsilon_0$ and $\mu_0$ are fundamental constants that appear in Maxwell’s equations, relating electric and magnetic fields to their sources (charge and current) and to each other. They also determine the speed of light in vacuum.

| Constant | Meaning | Value |
|---|---|---|
| $\varepsilon_0$ | Vacuum permittivity | $8.854 \times 10^{-12}\ \mathrm{F/m}$ |
| $\mu_0$ | Vacuum permeability | $4\pi \times 10^{-7}\ \mathrm{H/m}$ |
