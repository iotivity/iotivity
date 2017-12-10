IoTivity Build VM from Vagrant
==============================

Vagrant can use the files in this directory to provision an Ubuntu
16.04LTS development environment for IoTivity.  To create a new VM:

    % vagrant up

in the current directory.  The default provider on your given host
may not be supported by the box to be used, which will give an error.
If so specify a provider to use. e.g. to use virtualbox:

   % vagrant up --provider virtualbox

You can then connect to the VM with:

    % vagrant ssh

On the newly provisioned VM, the IoTivity git repo will be checked out
in the `iotivity` directory in the vagrant user's home directory.

Optional (currently disabled) features are provisioning for Android
building. Android will now correctly provision on first scons run where
those build options are selected. Arduino building is no longer supported
on iotivity (use iotivity-constrained instead).

Also optional, it should be able to configure for Ubuntu 17.04
and 17.10, by changing which of the the stanzas in Vagrantfile is
uncommented. Non-LTS Vagrant boxes vanish from the upstream as soon as
they go out of support (nine months for Ubuntu), so these sections will
go out of date quickly.

Using Vagrant to build a VM might be useful for:

* Testing the build process on a clean install to make sure all dependencies
  are captured

* Configuring a Linux build environment for Windows or Mac users

* Configuring VMs for build automation

The Vagrantfile has only virtualbox provider-specific options. To use
other provides such as vmware, libvirt, just copy this stanza and
update (feel free to submit a patch!).  Note the default 2gb RAM
leaves a working system, but not much spare in doing builds. Don't
do parallel builds on this without bumping the memory up. Look for:

  config.vm.provider "virtualbox" do |vb|

The Vagrantfile defaults to setting up bridged networking, so that the
new image will be visible on the network for running iotivity tests
against other iotivity machines. It will lead to a pausing question in
the provisioning. If this is not wanted, comment this line out:

  config.vm.network "public_network"

This directory will be available inside the box as /vagrant, so if
you need to add something visible inside the box, just dropping it
here should be sufficient (e.g. the proxy_config file mentioned in 
comments in iotivity-setup.sh).

For more information about Vagrant, please see https://docs.vagrantup.com/v2/

Assumptions
===========

* The ${HOME}/.ssh/ directory contains files config, id_rsa, id_rsa.pub,
  and known_hosts which when copied to the VM will allow connection to
  iotivity gerrit, which is where the master git repo lives. That implies
  access to gerrit has been set up already on this machine.  Sub-assumption:
  you don't mind these personal files being copied to the VM.

* Your git username is obtainable by doing "ssh -G gerrit.iotivity.org".
  It will fall back to the USER and then to USERNAME environment variables,
  which may or may not be usable.  This information is part of the setup
  (try "git remote -v" in the iotivity directory in the running box to see),
  but can easily be changed if the guess is wrong.

