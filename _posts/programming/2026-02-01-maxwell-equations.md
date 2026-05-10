---
layout: post
title: Maxwell’s Equations in Plain English
category: programming
description: A simple guide to Maxwell’s equations with daily-life and PCB design examples.
tags: [pcb, embedded, physics, electronics]
---

Maxwell’s equations describe how electric and magnetic fields behave.
If you work on embedded systems or PCB design, these equations explain many things you see in real hardware.

In this post, we keep it simple. For each equation, you get:

- a plain explanation,
- the differential form,
- the integral form,
- one everyday-life example,
- one PCB-design example.

* 
{:toc}

## 1) Gauss’s Law (Electric Field)

Gauss’s Law says electric charge is the source of electric field.
More charge means more electric field lines coming out (or going in).

### Differential form

$$
\nabla \cdot \mathbf{E} = \frac{\rho}{\varepsilon_0}
$$

Plain meaning: at a point in space, electric field “starts” or “ends” where charge exists.

### Integral form

$$
\oint_S \mathbf{E} \cdot d\mathbf{A} = \frac{Q_{\text{enc}}}{\varepsilon_0}
$$

Plain meaning: the total electric flux through a closed surface depends on how much charge is inside.

### Everyday-life example

When you rub a balloon on your hair, the balloon gets charge and attracts small paper pieces.
That attraction is a direct effect of electric field created by charge.

### PCB-design example

A charged copper area on a board creates an electric field around it.
That field couples to nearby traces and can add noise, especially on high-impedance signals.

---

## 2) Gauss’s Law for Magnetism

This law says magnetic field lines are always closed loops.
In simple words: no isolated magnetic “charge” has been observed.

### Differential form

$$
\nabla \cdot \mathbf{B} = 0
$$

Plain meaning: magnetic field does not begin or end at a point like electric field does.

### Integral form

$$
\oint_S \mathbf{B} \cdot d\mathbf{A} = 0
$$

Plain meaning: net magnetic flux through any closed surface is always zero.

### Everyday-life example

Cut a bar magnet in half and you do not get a single north pole and single south pole.
You get two smaller magnets, each with north and south.

### PCB-design example

Current loops on a PCB create magnetic fields that return through nearby paths.
If the return path is broken, loop area grows and radiated EMI usually gets worse.

---

## 3) Faraday’s Law of Induction

Faraday’s Law says a changing magnetic field creates an electric field.
This is the basis of generators, transformers, and many sensors.

### Differential form

$$
\nabla \times \mathbf{E} = -\frac{\partial \mathbf{B}}{\partial t}
$$

Plain meaning: when magnetic field changes over time, it induces a circulating electric field.

### Integral form

$$
\oint_C \mathbf{E} \cdot d\mathbf{l} = -\frac{d}{dt}\int_S \mathbf{B} \cdot d\mathbf{A}
$$

Plain meaning: changing magnetic flux through a loop creates voltage around that loop.

### Everyday-life example

A phone wireless charger uses changing magnetic fields to induce voltage in the phone coil.
That induced voltage is then used to charge the battery.

### PCB-design example

A fast-changing current in a power inductor can induce voltage in nearby traces.
If sensitive analog traces are too close, you can see switching noise in measurements.

---

## 4) Ampère–Maxwell Law

This law says magnetic fields are created by:

- electric current, and
- changing electric fields.

### Differential form

$$
\nabla \times \mathbf{B} = \mu_0 \mathbf{J} + \mu_0 \varepsilon_0 \frac{\partial \mathbf{E}}{\partial t}
$$

Plain meaning: current and time-varying electric fields both generate magnetic field circulation.

### Integral form

$$
\oint_C \mathbf{B} \cdot d\mathbf{l} = \mu_0 I_{\text{enc}} + \mu_0 \varepsilon_0 \frac{d}{dt}\int_S \mathbf{E} \cdot d\mathbf{A}
$$

Plain meaning: magnetic field around a closed path depends on enclosed current and changing electric flux.

### Everyday-life example

Current in a charging cable creates a magnetic field around the cable.
A clamp meter measures this field to estimate current.

### PCB-design example

High di/dt currents in buck converters create strong local magnetic fields.
Keeping loops short and placing decoupling capacitors close helps reduce EMI.

---

## Quick note: from field equations to Ohm’s law (intuition)

For conductors, current density is related to electric field:

$$
\mathbf{J} = \sigma \mathbf{E}
$$

If the wire is uniform, this leads to:

$$
R = \frac{L}{\sigma A}
$$

This is a field-based way to understand resistance: longer path means higher resistance, larger cross-section means lower resistance.

---

## Useful constants (SI)

$$
\varepsilon_0 \approx 8.854 \times 10^{-12}\ \mathrm{F/m}
$$

$$
\mu_0 = 4\pi \times 10^{-7}\ \mathrm{H/m}
$$

These constants connect electric and magnetic fields in Maxwell’s equations.

---

## Final summary

Maxwell’s equations are not only theory.
They explain practical things you see every day and while designing PCBs:

- charge creates electric fields,
- magnetic fields form loops,
- changing magnetic fields induce voltage,
- currents (and changing electric fields) create magnetic fields.

If you remember these four ideas, many hardware behaviors become easier to predict and debug.
