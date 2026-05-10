---
layout: post
title: Maxwell's Equations
categories: programming
description: Summary of Maxwell's equations and their relevance to PCB design and embedded systems.
tags: [pcb, embedded, physics]
---

Maxwell’s equations describe how electric and magnetic fields behave. If you work on embedded systems or PCB design, these equations explain many things you see in real hardware.

* 
{:toc}

# 0. Divergence and Curl

Let's quickly review two important vector calculus concepts that appear in Maxwell’s equations:

- **Divergence** ($\nabla \cdot$) measures how much a vector field spreads out from a point. Positive divergence means field lines are spreading out (like from a positive charge), while negative divergence means they are converging (like towards a negative charge).
- **Curl** ($\nabla \times$) measures how much a vector field rotates around a point. A nonzero curl means the field has a swirling or rotational component.

---

# 1. Gauss’s Law

Describes how an electric charge influences the electric field around it.


## Differential Form

Describes exactly how a single charge affects the electric field around it.

It tells us how much the electric field **diverges** from a charge.
Think of it like gravity: if divergence is **positive**, field lines spread out; if negative, they converge.

$$
\nabla \cdot \mathbf{E} = \frac{\rho}{\varepsilon_0}
$$

| Symbol | Meaning | SI Unit |
|---|---|---|
| $\mathbf{E}$ | Electric field | $\mathrm{V/m}$ |
| $\rho$ | Charge density | $\mathrm{C/m^3}$ |
| $\varepsilon_0$ | Vacuum permittivity | $\mathrm{F/m}$ |

## Integral Form

Describes how much total electric “flux” passes through a closed surface due to enclosed charge.

In simple terms: how much “influence” a charge has on its surroundings.

$$
\oint_S \mathbf{E} \cdot d\mathbf{A}
=
\frac{Q_{\text{enc}}}{\varepsilon_0}
$$

| Symbol | Meaning | SI Unit |
|---|---|---|
| $Q_{\text{enc}}$ | Enclosed charge | $\mathrm{C}$ |
| $d\mathbf{A}$ | Surface area element | $\mathrm{m^2}$ |

## PCB example
- If you have a floating copper island on a PCB, it can accumulate charge and create unwanted coupling with nearby signal traces.
- On a PCB, a single via or pad connected to a high voltage net creates an electric field around it, affecting nearby traces (especially in high-impedance analog circuits).

## How Electric Fields relate to Currents

Current density $\mathbf{J}$ is related to electric field $\mathbf{E}$:

$$
\mathbf{J} = \sigma \mathbf{E}
$$

where $\sigma$ is conductivity.

- High $\sigma$ (copper trace) → current flows easily
- Low $\sigma$ (FR4) → almost no current flow

Total current:

$$
I = \int_S \mathbf{J} \cdot d\mathbf{A}
$$

Substituting:

$$
I = \int_S \sigma \mathbf{E} \cdot d\mathbf{A}
$$

Assuming uniform electrical field and area:

$$
I = \sigma E A
$$

Voltage and electric field:


$$
V_{ab} = -\int_a^b \mathbf{E} \cdot d\mathbf{l}
$$

Assuming a uniform electric field along the length of the wire:

$$
V = E \cdot L \implies E = \frac{V}{L}
$$

Substitute into current:

$$
I = \sigma \frac{V}{L} A
$$

Resistance:

$$
R = \frac{V}{I} = \frac{L}{\sigma A}
$$

**PCB example:**
- Long thin trace → higher resistance
- Wide copper pour → lower resistance
- Power traces are made wide to reduce voltage drop

---

# 2. Gauss’s Law for Magnetism

Describes how magnetic fields behave.

## Differential Form

A single "magnetic charge" would have a north and south pole, so the field lines that "diverge" from the north pole would "converge" at the south pole, resulting in zero net divergence. That's why there are no magnetic monopoles.

$$
\nabla \cdot \mathbf{B} = 0
$$

| Symbol | Meaning | SI Unit |
|---|---|---|
| $\mathbf{B}$ | Magnetic field | $\mathrm{T}$ |


## Integral Form

Total magnetic flux through any closed surface is zero, since all magnetic field lines that cross the surface must enter and exit, resulting in no net flux.

$$
\oint_S \mathbf{B} \cdot d\mathbf{A} = 0
$$

---

# 3. Faraday’s Law of Induction

Describes how changing magnetic fields generate electric fields.

## Differential Form

A changing magnetic field creates a circulating electric field.
It tells us how easy is to make the electric field $\mathbf{E}$ rotate around a point due to a changing magnetic field over time.
The direction of rotation is given by the right-hand rule, and the negative sign indicates that the induced electric field opposes the change in magnetic flux.

$$
\nabla \times \mathbf{E}
=
-\frac{\partial \mathbf{B}}{\partial t}
$$

| Symbol | Meaning | SI Unit |
|---|---|---|
| $\mathbf{E}$ | Electric field | $\mathrm{V/m}$ |
| $\mathbf{B}$ | Magnetic field | $\mathrm{T}$ |

## Integral Form

A changing magnetic flux induces voltage (EMF - electromotive force) in a loop. This is the principle behind transformers and inductors.
If you connect a resistor to that loop, it will generate a current due to the induced EMF.

$$
\oint_C \mathbf{E} \cdot d\mathbf{l}
=
-
\frac{d}{dt}
\int_S \mathbf{B} \cdot d\mathbf{A}
$$

| Symbol | Meaning | SI Unit |
|---|---|---|
| $d\mathbf{l}$ | Line element | $\mathrm{m}$ |
| $\mathbf{B}\cdot d\mathbf{A}$ | Magnetic flux | $\mathrm{Wb}$ |

## PCB example:
- Any wire or trace can become an antenna if it has a changing magnetic field around it. The longer the traces and the higher the frequencies of the magnetic field, the worse the antenna effect.

---

# 4. Ampère–Maxwell Law

Describes how electric currents and changing electric fields generate magnetic fields.
Important for Electromagnetic Interference (EMI) in PCB design.

## Differential Form

This form tells us that magnetic fields are created by two things:

- Electric current flowing in a wire generates a magnetic field around it.
- A changing electric field over time. Even if no current is physically flowing, a changing electric field can still create a magnetic field. This is why capacitors in circuits still allow AC signals to pass.

It also indicates the direction of rotation of the magnetic field around the current or changing electric field, following the right-hand rule.

$$
\nabla \times \mathbf{B}
=
\mu_0 \mathbf{J}
+
\mu_0 \varepsilon_0
\frac{\partial \mathbf{E}}{\partial t}
$$

$$
\nabla \times \mathbf{B}
=
\mu_0 \mathbf{\sigma} \mathbf{E}
+
\mu_0 \varepsilon_0
\frac{\partial \mathbf{E}}{\partial t}
$$

| Symbol | Meaning | SI Unit |
|---|---|---|
| $\mathbf{B}$ | Magnetic field | $\mathrm{T}$ |
| $\mathbf{J}$ | Current density | $\mathrm{A/m^2}$ |
| $\mu_0$ | Vacuum permeability | $\mathrm{H/m}$ |
| $\varepsilon_0$ | Vacuum permittivity | $\mathrm{F/m}$ |


## Integral Form

If you measure the magnetic field around a closed loop (like around a trace or wire),

You will find it depends on:
- The current passing through that loop
- Plus the changing electric field inside it

So, Magnetic fields “circulate” around currents, but also around changing electric fields.


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

## PCB example
- The integral form explains why if you have two parallel traces that carry the same current in opposite directions, their magnetic fields will cancel each other out, reducing EMI. 
- High-speed digital traces (SPI, USB, clocks) generate time-varying fields that can radiate EMI and also cause crosstalk with nearby traces. That's why it's important to keep them short and well-routed.
- Current loops on a PCB create magnetic fields that can couple into nearby analog traces. That's why if a trace carries a high current, it's best to route it away from sensitive analog circuits.

---

# Useful Physical Constants (SI)

$$
\varepsilon_0 \approx 8.854 \times 10^{-12}\ \mathrm{F/m}
$$

$$
\mu_0 = 4\pi \times 10^{-7}\ \mathrm{H/m}
$$

These constants define how electric and magnetic fields behave in vacuum and determine the speed of light.

| Constant | Meaning | Value |
|---|---|---|
| $\varepsilon_0$ | Vacuum permittivity | $8.854 \times 10^{-12}\ \mathrm{F/m}$ |
| $\mu_0$ | Vacuum permeability | $4\pi \times 10^{-7}\ \mathrm{H/m}$ |
