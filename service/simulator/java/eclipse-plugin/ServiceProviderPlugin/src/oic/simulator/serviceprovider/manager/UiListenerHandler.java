/*
 * Copyright 2015 Samsung Electronics All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package oic.simulator.serviceprovider.manager;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import oic.simulator.serviceprovider.listener.IAutomationUIListener;
import oic.simulator.serviceprovider.listener.IObserverListChangedUIListener;
import oic.simulator.serviceprovider.listener.IPropertiesChangedUIListener;
import oic.simulator.serviceprovider.listener.IResourceListChangedUIListener;
import oic.simulator.serviceprovider.listener.IResourceModelChangedUIListener;
import oic.simulator.serviceprovider.listener.ISelectionChangedUIListener;
import oic.simulator.serviceprovider.model.Device;
import oic.simulator.serviceprovider.model.Resource;
import oic.simulator.serviceprovider.model.ResourceType;
import oic.simulator.serviceprovider.model.SingleResource;

public class UiListenerHandler {

    private static UiListenerHandler              uiHandler;

    private List<IResourceListChangedUIListener>  resourceListChangedUIListeners;

    private List<ISelectionChangedUIListener>     selectionChangedUIListeners;

    private List<IResourceModelChangedUIListener> resourceModelChangedUIListeners;

    private List<IAutomationUIListener>           automationUIListeners;

    private List<IObserverListChangedUIListener>  observerUIListeners;

    private List<IPropertiesChangedUIListener>    propertiesChangedUIListeners;

    private UiListenerHandler() {
        resourceListChangedUIListeners = new ArrayList<IResourceListChangedUIListener>();
        selectionChangedUIListeners = new ArrayList<ISelectionChangedUIListener>();
        resourceModelChangedUIListeners = new ArrayList<IResourceModelChangedUIListener>();
        automationUIListeners = new ArrayList<IAutomationUIListener>();
        observerUIListeners = new ArrayList<IObserverListChangedUIListener>();
        propertiesChangedUIListeners = new ArrayList<IPropertiesChangedUIListener>();
    }

    public static UiListenerHandler getInstance() {
        if (null == uiHandler) {
            uiHandler = new UiListenerHandler();
        }
        return uiHandler;
    }

    public void addResourceListChangedUIListener(
            IResourceListChangedUIListener resourceListChangedUIListener) {
        synchronized (resourceListChangedUIListeners) {
            resourceListChangedUIListeners.add(resourceListChangedUIListener);
        }
    }

    public void addResourceSelectionChangedUIListener(
            ISelectionChangedUIListener resourceSelectionChangedUIListener) {
        synchronized (selectionChangedUIListeners) {
            selectionChangedUIListeners.add(resourceSelectionChangedUIListener);
        }
    }

    public void addResourceModelChangedUIListener(
            IResourceModelChangedUIListener resourceModelChangedUIListener) {
        synchronized (resourceModelChangedUIListeners) {
            resourceModelChangedUIListeners.add(resourceModelChangedUIListener);
        }
    }

    public void addAutomationUIListener(
            IAutomationUIListener automationUIListener) {
        synchronized (automationUIListeners) {
            automationUIListeners.add(automationUIListener);
        }
    }

    public void addObserverListChangedUIListener(
            IObserverListChangedUIListener observerListChangedUIListener) {
        synchronized (observerUIListeners) {
            observerUIListeners.add(observerListChangedUIListener);
        }
    }

    public void addResourcePropertiesChangedUIListener(
            IPropertiesChangedUIListener resourcePropertiesChangedUIListener) {
        synchronized (propertiesChangedUIListeners) {
            propertiesChangedUIListeners
                    .add(resourcePropertiesChangedUIListener);
        }
    }

    public void removeResourceListChangedUIListener(
            IResourceListChangedUIListener listener) {
        synchronized (resourceListChangedUIListeners) {
            if (null != listener && resourceListChangedUIListeners.size() > 0) {
                resourceListChangedUIListeners.remove(listener);
            }
        }
    }

    public void removeResourceSelectionChangedUIListener(
            ISelectionChangedUIListener listener) {
        synchronized (selectionChangedUIListeners) {
            if (null != listener && selectionChangedUIListeners.size() > 0) {
                selectionChangedUIListeners.remove(listener);
            }
        }
    }

    public void removeResourceModelChangedUIListener(
            IResourceModelChangedUIListener listener) {
        synchronized (resourceModelChangedUIListeners) {
            if (null != listener && resourceModelChangedUIListeners.size() > 0) {
                resourceModelChangedUIListeners.remove(listener);
            }
        }
    }

    public void removeAutomationUIListener(IAutomationUIListener listener) {
        synchronized (automationUIListeners) {
            if (null != listener && automationUIListeners.size() > 0) {
                automationUIListeners.remove(listener);
            }
        }
    }

    public void removeObserverListChangedUIListener(
            IObserverListChangedUIListener listener) {
        synchronized (observerUIListeners) {
            if (null != listener && observerUIListeners.size() > 0) {
                observerUIListeners.remove(listener);
            }
        }
    }

    public void removeResourcePropertiesChangedUIListener(
            IPropertiesChangedUIListener resourcePropertiesChangedUIListener) {
        synchronized (propertiesChangedUIListeners) {
            propertiesChangedUIListeners
                    .remove(resourcePropertiesChangedUIListener);
        }
    }

    public void resourceCreatedUINotification(ResourceType type) {
        synchronized (resourceListChangedUIListeners) {
            if (resourceListChangedUIListeners.size() > 0) {
                IResourceListChangedUIListener listener;
                Iterator<IResourceListChangedUIListener> listenerItr = resourceListChangedUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onResourceCreation(type);
                    }
                }
            }
        }
    }

    public void resourceDeletedUINotification(ResourceType type) {
        synchronized (resourceListChangedUIListeners) {
            if (resourceListChangedUIListeners.size() > 0) {
                IResourceListChangedUIListener listener;
                Iterator<IResourceListChangedUIListener> listenerItr = resourceListChangedUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onResourceDeletion(type);
                    }
                }
            }
        }
    }

    public void resourceListUpdateUINotification(ResourceType type) {
        synchronized (resourceListChangedUIListeners) {
            if (resourceListChangedUIListeners.size() > 0) {
                IResourceListChangedUIListener listener;
                Iterator<IResourceListChangedUIListener> listenerItr = resourceListChangedUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onResourceListUpdate(type);
                    }
                }
            }
        }
    }

    public void resourceSelectionChangedUINotification(Resource resource) {
        synchronized (selectionChangedUIListeners) {
            if (selectionChangedUIListeners.size() > 0) {
                ISelectionChangedUIListener listener;
                Iterator<ISelectionChangedUIListener> listenerItr = selectionChangedUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onResourceSelectionChange(resource);
                    }
                }
            }
        }
    }

    public void deviceSelectionChangedUINotification(Device dev) {
        synchronized (selectionChangedUIListeners) {
            if (selectionChangedUIListeners.size() > 0) {
                ISelectionChangedUIListener listener;
                Iterator<ISelectionChangedUIListener> listenerItr = selectionChangedUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onDeviceSelectionChange(dev);
                    }
                }
            }
        }
    }

    public void resourceModelChangedUINotification(Resource resource) {
        synchronized (resourceModelChangedUIListeners) {
            if (resourceModelChangedUIListeners.size() > 0 && null != resource) {
                IResourceModelChangedUIListener listener;
                Iterator<IResourceModelChangedUIListener> listenerItr = resourceModelChangedUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onResourceModelChange(resource);
                    }
                }
            }
        }
    }

    public void resourceAutomationStartedUINotification(SingleResource resource) {
        synchronized (automationUIListeners) {
            if (automationUIListeners.size() > 0 && null != resource) {
                IAutomationUIListener listener;
                Iterator<IAutomationUIListener> listenerItr = automationUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onResourceAutomationStart(resource);
                    }
                }
            }
        }
    }

    public void automationCompleteUINotification(SingleResource resource,
            String attName) {
        synchronized (automationUIListeners) {
            if (automationUIListeners.size() > 0 && null != resource) {
                IAutomationUIListener listener;
                Iterator<IAutomationUIListener> listenerItr = automationUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onAutomationComplete(resource, attName);
                    }
                }
            }
        }
    }

    public void observerListChangedUINotification(Resource resource) {
        synchronized (observerUIListeners) {
            if (observerUIListeners.size() > 0 && null != resource) {
                IObserverListChangedUIListener listener;
                Iterator<IObserverListChangedUIListener> listenerItr = observerUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        listener.onObserverListChanged(resource);
                    }
                }
            }
        }
    }

    public void propertiesChangedUINotification(Class targetClass) {
        synchronized (propertiesChangedUIListeners) {
            if (propertiesChangedUIListeners.size() > 0) {
                IPropertiesChangedUIListener listener;
                Iterator<IPropertiesChangedUIListener> listenerItr = propertiesChangedUIListeners
                        .iterator();
                while (listenerItr.hasNext()) {
                    listener = listenerItr.next();
                    if (null != listener) {
                        if (targetClass == Resource.class) {
                            listener.onResourcePropertyChange();
                        } else {
                            listener.onDevicePropertyChange();
                        }
                    }
                }
            }
        }
    }
}
