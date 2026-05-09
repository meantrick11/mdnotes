# Brute Force

## Description

A brute force attack can manifest itself in many different ways, **but primarily consists in an attacker configuring predetermined values, making requests to a server using those values, and then analyzing the response.** For the sake of efficiency, an attacker may use a dictionary attack (with or without mutations) or a traditional brute-force attack (with given classes of characters e.g.: alphanumeric, special, case (in)sensitive). Considering a given method, number of tries, efficiency of the system which conducts the attack, and estimated efficiency of the system which is attacked the attacker is able to calculate approximately how long it will take to submit all chosen predetermined values.

## examples

example1(**目录爆破**):
A web application can be attacked via brute force by taking a word list of known pages, for instance from a popular content management system, and simply requesting each known page then analyzing the HTTP response code to determine if the page exists on the target server.

example 2(**认证爆破**):
In regards to authentication, when no password policy is in place an attacker can use lists of common username and passwords to brute force a username or password field until successful authentication.

## tools:

### DirBuster

### dirb

### WebRoot

### BurpSuite

### Ffuf

## Defensive Tools

### Php-Vrute-Force-attack Detector

