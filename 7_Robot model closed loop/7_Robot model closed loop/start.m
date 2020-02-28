clear all
clc

fs = 100;

FLC1 = readfis('FLC1');
FLC2 = readfis('FLC2');
FLC3 = readfis('FLC3');

open('robot_model')
sim('robot_model')