/*
 *******************************************************************
 *
 * Copyright 2018 Intel Corporation.
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
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
 *
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 */

package org.iotivity.base.examples;

import java.awt.Component;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.Map;
import java.util.Observable;
import java.util.Observer;
import java.util.concurrent.ConcurrentHashMap;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.JSpinner;
import javax.swing.SpinnerNumberModel;
import javax.swing.SwingUtilities;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 * Light Control Panel
 */
public class LightControlPanel extends JPanel implements Observer {

    private JCheckBox powerCheckBox;
    private SpinnerNumberModel dimmingValueModel;

    private JCheckBox slowResponseCheckBox;

    public LightControlPanel(boolean powerOn, int dimmingValue) {
        setLayout(new GridBagLayout());

        JLabel switchLabel = new JLabel("Binary Switch: ");
        powerCheckBox = new JCheckBox(powerOn ? "On" : "Off", powerOn);
        powerCheckBox.addChangeListener(new ChangeListener() {
            @Override
            public void stateChanged(ChangeEvent changeEvent) {
                powerCheckBox.setText(powerCheckBox.isSelected() ? "On" : "Off");
            }
        });
        powerCheckBox.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                notifySwitchObservers();
            }
        });
        add(switchLabel, new GridBagConstraints(0, 0, 1, 1, 0.0, 0.0, GridBagConstraints.WEST, GridBagConstraints.NONE, new Insets(4,4,4,4), 0, 0));
        add(powerCheckBox, new GridBagConstraints(1, 0, 1, 1, 0.0, 0.0, GridBagConstraints.WEST, GridBagConstraints.NONE, new Insets(4,4,4,4), 0, 0));

        JLabel dimmingLabel = new JLabel("Dimming Setting: ");
        dimmingValueModel = new SpinnerNumberModel(dimmingValue, 0, 100, 1);
        JSpinner dimmingValueSpinner = new JSpinner(dimmingValueModel);
        // only allow keypad arrows or mouse clicks
        ((JSpinner.DefaultEditor) dimmingValueSpinner.getEditor()).getTextField().setEditable(false);
        ((JSpinner.DefaultEditor) dimmingValueSpinner.getEditor()).getTextField().addKeyListener(new KeyListener() {
            @Override
            public void keyTyped(KeyEvent e) {
            }
            @Override
            public void keyPressed(KeyEvent e) {
            }
            @Override
            public void keyReleased(KeyEvent e) {
                int code = e.getKeyCode();
                if ((code == KeyEvent.VK_UP) || (code == KeyEvent.VK_KP_UP) || (code == KeyEvent.VK_DOWN) || (code == KeyEvent.VK_KP_DOWN)) {
                    notifyDimmingObservers();
                }
            }
        });
        Component[] components = dimmingValueSpinner.getComponents();
        for (Component component : components) {
            if (component instanceof JButton) {
                ((JButton)component).addActionListener(new ActionListener() {
                    @Override
                    public void actionPerformed(ActionEvent e) {
                        notifyDimmingObservers();
                    }
                });
            }
        }
        add(dimmingLabel, new GridBagConstraints(0, 1, 1, 1, 0.0, 0.0, GridBagConstraints.WEST, GridBagConstraints.NONE, new Insets(4,4,4,4), 0, 0));
        add(dimmingValueSpinner, new GridBagConstraints(1, 1, 1, 1, 0.0, 0.0, GridBagConstraints.WEST, GridBagConstraints.NONE, new Insets(4,4,4,4), 0, 0));

        add(new JSeparator(), new GridBagConstraints(0, 2, 2, 1, 0.0, 0.0, GridBagConstraints.WEST, GridBagConstraints.HORIZONTAL, new Insets(6,4,4,4), 0, 0));

        slowResponseCheckBox = new JCheckBox("Send Slow (Asynchronous) Responses");
        add(slowResponseCheckBox, new GridBagConstraints(0, 3, 2, 1, 0.0, 0.0, GridBagConstraints.WEST, GridBagConstraints.NONE, new Insets(4,4,4,4), 0, 0));

        add(new JPanel(), new GridBagConstraints(0, 4, 2, 1, 1.0, 1.0, GridBagConstraints.CENTER, GridBagConstraints.BOTH, new Insets(0,0,0,0), 0, 0));
    }

    public boolean useSlowResponse() {
        return slowResponseCheckBox.isSelected();
    }

    private void setPowerOn(boolean powerOn) {
        powerCheckBox.setSelected(powerOn);
    }

    private void setDimmingValue(int dimmingValue) {
        dimmingValueModel.setValue(dimmingValue);
    }

    @Override
    public void update(Observable o, Object arg) {
        boolean repaintRequired = false;
        if (o instanceof Switch) {
            boolean powerOn = ((Switch) o).getValue();
            setPowerOn(powerOn);
            repaintRequired = true;

        } else if (o instanceof Dimming) {
            int dimmingValue = ((Dimming) o).getDimmingValue();
            setDimmingValue(dimmingValue);
            repaintRequired = true;

        } else {
            // ignore
        }

        if (repaintRequired) {
            SwingUtilities.invokeLater(new Runnable() {
                @Override
                public void run() {
                    repaint();
                }
            });
        }
    }

    // LightControlObservable implementation
    private Map<Integer, LightControlSwitchObserver> switchObservers = new ConcurrentHashMap<Integer, LightControlSwitchObserver>();
    private Map<Integer, LightControlDimmingObserver> dimmingObservers = new ConcurrentHashMap<Integer, LightControlDimmingObserver>();

    public void addObserver(LightControlSwitchObserver observer) {
        switchObservers.put(observer.hashCode(), observer);
    }

    public void addObserver(LightControlDimmingObserver observer) {
        dimmingObservers.put(observer.hashCode(), observer);
    }

    private void notifySwitchObservers() {
        Thread observerNotifier = new Thread(new Runnable() {
            public void run() {
                for (LightControlSwitchObserver observer : switchObservers.values()) {
                    observer.update(powerCheckBox.isSelected());
                }
            }
        });
        observerNotifier.setDaemon(true);
        observerNotifier.start();
    }

    private void notifyDimmingObservers() {
        Thread observerNotifier = new Thread(new Runnable() {
            public void run() {
                for (LightControlDimmingObserver observer : dimmingObservers.values()) {
                    observer.update(dimmingValueModel.getNumber().intValue());
                }
            }
        });
        observerNotifier.setDaemon(true);
        observerNotifier.start();
    }
}
