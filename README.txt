EGC - Tema 3
Ski Jumping
Dinu Marian-Alexandru 334 CC

1.Cerinta

Am avut de implementat un joc o scena pt sarituri cu schiurile. In scena se aflau 2 trambuline,
2 jucatori activi, jucatori inactivi, un quad, camere.

2. Utilizare

C = schimbare camera
Atunci cand camera FP de sus este activa:
1 = zoom in 
2 = zoom out
3 = deplasare stanga
4 = deplasare dreapta
5 = deplasare sus
6 = deplasare down
7 = rotatie Oz 
8 = rotatie Oz
9 = rotatie Oy
0 = rotatie Oy
- = rotatie Ox
= = rotatie Ox
E = start cutremur 
R = stop cutremur
W = modul wireframe
S = modul solid
ESC = inchiderea ferestrei de joc

3. Implementare
IDE - VS 2012
Compilator - GCC 4.5.2
SO - Windows 8.1

5. Probleme aparute
Rotatia playerului + miscarea camerelor FP in functie de noua directie.

6. Continutul arhivei
Am pornit de la Framework-ul din laboratorul 5 si am adaugat cod in main 
+ intr-o clasa lab_camera. In main sunt desenate toate obiectele si este 
implementata si dinamica jocului. In lab_camera sunt implementate camerele.

7. Functionalitati
Functionalitati Standard (ca in enunt):
Tot, mai putin rotatia player-ului si aglomerarea jucatorilor inactivi pe quad.
Jucatorii aluneca pe trambulina(planul inclinat), apoi executa actiunea de JUMP
(zboara avand traiectoria unui semicerc), apoi cad lin pe quad, unde apoi se misca liber
pana isi termina miscarea.
Personajul principal isi schimba culoarea dupa formula din enunt. Pentru normalizare am
folosit fabs(cos(timer + 100)). In timpul cutremurului punctele care apartin personajului
principal si trambulinei pe care se afla acesta sunt perturbate folosind functia cosinus, 
mai exact (cos(timer - 100), astfel se asigura intestati diferite ale cutremurului. 
(timer = timpul, care se incrementeaza la fiecare iteratie);
