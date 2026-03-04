Pour utiliser mpi:

sudo apt-get  install  libopenmpi-dev  openmpi-bin


il faut le fichier sn_resamp512 à la  position indiquée dans le CMakeList.txt.


Puis une fois toutes le manipulations cmake faites vous pouvez lancer le code avec cette commande mais il est possible que vous soyez contraint de mettre plus de coeur sur votre VM:

make prun1   

Elle lance un  processus mpi. Elle génère une seule image. 

"make prun2" lance 2 processus

"make prun4" lance 4 processus 

Autre lancement possible du programme:

mpirun -np 2 ./vtktp
mpirun -np 4 ./vtktp
mpirun -np 8 ./vtktp

Pour lancer avec moins de coeur utilisez cette commande qui indique à MPI de skeduler les process sur un seul noeud:

mpirun -np 2 --host localhost:16 vtktp 
mpirun -np 8 --host localhost:16 vtktp 
mpirun -np 16 --host localhost:16 vtktp 


