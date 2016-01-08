#This script will perform the prerequisits steps to build CTT GUI

check_if_java8_installed()
{
	if type -p java; then
	    echo found java executable in PATH
	    _java=java
	else
	    echo "Java is not installed"
	    return 0
	fi


	if [[ "$_java" ]]; then
	    version=$("$_java" -version 2>&1 | awk -F '"' '/version/ {print $2}')
	    echo version "$version"
	    if [[ "$version" = *"1.8"* ]]; then
		echo Java version is 1.8
		return 1
	    else         
		echo Java version is not 1.8
		return 0
	    fi
	fi

}

install_java8()
{
	sudo add-apt-repository ppa:webupd8team/java
	sudo apt-get update
	sudo apt-get install oracle-java8-installer
}


check_if_gradle_installed()
{
	if type -p gradle; then
	    echo "gradle installed"
	    return 1
	else
	    echo "gradle is not installed"
	    return 0
	fi
}

install_gradle()
{
	sudo add-apt-repository ppa:cwchien/gradle
	sudo apt-get update
	sudo apt-get install gradle
}
check_if_jython_installed()
{
	if type -p jython; then
	    echo "jython installed"
	    return 1
	else
	    echo "jython is not installed"
	    return 0
	fi
}
install_jython()
{
	wget https://repo1.maven.org/maven2/org/python/jython-installer/2.7.0/jython-installer-2.7.0.jar
	echo "Do not change the path of default jython install path"
	java -jar jython-installer-2.7.0.jar
	echo "export PATH=\$PATH:/home/$USER/jython2.7.0/bin" >> /home/$USER/.bashrc
	. ~/.bashrc
}

install_robotframework()
{
	git clone https://github.com/robotframework/robotframework.git
	cd robotframework
	jython setup.py install
	cd ..
}

check_if_tshark_installed()
{
	if type -p tshark; then
	    echo "tshark installed"
	    return 1
	else
	    echo "tshark is not installed"
	    return 0
	fi
}
install_tshark()
{
	sudo apt-get install tshark
	sudo chmod 4711 `which dumpcap`
}

check_if_maven_installed()
{
	if type -p mvn; then
	    echo "maven installed"
	    return 1
	else
	    echo "maven is not installed"
	    return 0
	fi
}


check_if_robot_installed()
{
	if type -p jybot; then
	    echo "jybot installed"
	    return 1
	else
	    echo "jybot is not installed"
	    return 0
	fi
}

install_maven()
{
	wget www.eu.apache.org/dist/maven/maven-3/3.3.9/binaries/apache-maven-3.3.9-bin.tar.gz
	cp  apache-maven-3.3.9-bin.tar.gz ~/
	cd ~/
	tar xzvf apache-maven-3.3.9-bin.tar.gz
	echo "export PATH=\$PATH:/home/$USER/apache-maven-3.3.9/bin" >> ~/.bashrc
	. ~/.bashrc
}

check_if_java8_installed
javaInstall=$?

if [ $javaInstall == 0 ]; then
	install_java8
fi

check_if_jython_installed
jythonInstall=$?
if [ $jythonInstall == 0 ]; then
	install_jython
fi

check_if_robot_installed
jybotInstall=$?
if [ $jybotInstall == 0 ]; then
	install_robotframework
fi

check_if_tshark_installed
tsarkInstall=$?
if [ $tsarkInstall == 0 ]; then
	install_tshark
fi

check_if_maven_installed
mavenInstall=$?
if [ $mavenInstall == 0 ]; then
	install_maven
fi

check_if_gradle_installed
gradleInstall=$?
if [ $gradleInstall == 0 ]; then
	install_gradle
fi
