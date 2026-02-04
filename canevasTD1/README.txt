Pour compiler ce caneva:

apt update

apt install -y libvtk9-dev cmake libvtk9-qt-dev 



puis: 
	mkdir BUILD
	cd BUILD
	cmake ..

puis: 
	make 

Pour executer:

./vtktp


--------Mac OS

installer brew si pas encore fait

 /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"     



brew install cmake
brew install vtk


puis: 
	mkdir BUILD
	cd BUILD
	cmake ..

puis: 
	make 

Pour executer:

./vtktp


-----------------wibndows
-Installer wsl2 depuis le Microsoft Store (si vous avez wsl vous devez passer en wsl2 )
-Installer Ubuntu depuis le Microsoft Store
-Installer VcXsrv sur Windows https://sourceforge.net/projects/vcxsrv/
-Lancer Ubuntu depuis la barre de recherche Windows
-mettre à jours les packages "sudo apt update && sudo apt upgrade -y"
-installer les packages : sudo apt install mesa-utils libglu1-mesa-dev freeglut3-dev mesa-common-dev
-Modifier le .bashrc, "sudo nano .bashrc" et ajouter à la fin:
export DISPLAY=$(ip route list default | awk '{print $3}'):0
export LIBGL_ALWAYS_INDIRECT=0
cela permettra le transfert des fenêtres depuis VcXsrv
-relancer le terminal pour appliquer les changements
-sur Windows Lancer Xlaunch, choisir "multiple windows" > "Start no client" > décocher "Native opengl" et cocher "Disable acces control"
-installer les application x11 : sudo apt install x11-apps -y
-sur le terminal ubuntu lancer "xclock" si l'horloge apparait le programme fonctionne
-suivre les instructions du README.txt du TD1 "sudo apt install -y libvtk9-dev cmake libvtk9-qt-dev" 
-faire ensuite comme sous linux

