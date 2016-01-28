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

package org.oic.simulator;

/**
 * This class holds a set of methods for validating the attribute values of
 * different types using the given value properties {@link AttributeProperty}.
 */
public class AttributeValueValidation implements
        AttributeValueVisitor.VisitingMethods<Boolean> {
    private AttributeProperty mProperty = null;

    /**
     * Constructs {@link AttributeValueValidation} with the given attribute
     * value property.
     *
     * @param property
     *            Attribute value property.
     */
    public AttributeValueValidation(AttributeProperty property) {
        mProperty = property;
    }

    /**
     * API to validate the given attribute value. The given value is said to be
     * valid if it matches with the values given in the value property.
     *
     * @param value
     *            {@link AttributeValue} to be validated.
     * @return True if the given value is valid, otherwise false.
     */
    public boolean validate(AttributeValue value) {
        AttributeValueVisitor visitor = new AttributeValueVisitor(value, this);
        Boolean result = (Boolean) visitor.visit();
        return result.booleanValue();
    }

    /**
     * API to validate an Integer value.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(Integer value) {
        if (mProperty == null)
            return false;

        if (checkRange(value.doubleValue()) || checkValueSet(value))
            return true;
        return false;
    }

    /**
     * API to validate a Double value.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(Double value) {
        if (mProperty == null)
            return false;

        if (checkRange(value.doubleValue()) || checkValueSet(value))
            return true;
        return false;
    }

    /**
     * API to validate a Boolean value.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(Boolean value) {
        if (mProperty == null)
            return false;

        if (checkValueSet(value))
            return true;
        return false;
    }

    /**
     * API to validate a String value.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(String value) {
        if (mProperty == null)
            return false;

        if (checkRange(value.length()) || checkValueSet(value))
            return true;
        return false;
    }

    /**
     * API to validate a {@link SimulatorResourceModel} value.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(SimulatorResourceModel value) {
        return false;
    }

    /**
     * API to validate an array of integer values.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(Integer[] values) {
        if (mProperty == null)
            return false;

        if (!checkRange(values.length))
            return false;

        if (mProperty.getChildProperty() != null) {
            AttributeValueValidation rangeValidation = new AttributeValueValidation(
                    mProperty.getChildProperty());
            for (Integer value : values) {
                if (rangeValidation.visitingValue(value) == false)
                    return false;
            }
        }

        return true;
    }

    /**
     * API to validate an array of double values.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(Double[] values) {
        if (mProperty == null)
            return false;

        if (!checkRange(values.length))
            return false;

        if (mProperty.getChildProperty() != null) {
            AttributeValueValidation rangeValidation = new AttributeValueValidation(
                    mProperty.getChildProperty());
            for (Double value : values) {
                if (rangeValidation.visitingValue(value) == false)
                    return false;
            }
        }

        return true;
    }

    /**
     * API to validate an array of boolean values.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(Boolean[] values) {
        if (mProperty == null)
            return false;

        if (!checkRange(values.length))
            return false;

        if (mProperty.getChildProperty() != null) {
            AttributeValueValidation rangeValidation = new AttributeValueValidation(
                    mProperty.getChildProperty());
            for (Boolean value : values) {
                if (rangeValidation.visitingValue(value) == false)
                    return false;
            }
        }

        return true;
    }

    /**
     * API to validate an array of string values.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(String[] values) {
        if (mProperty == null)
            return false;

        if (!checkRange(values.length))
            return false;

        if (mProperty.getChildProperty() != null) {
            AttributeValueValidation rangeValidation = new AttributeValueValidation(
                    mProperty.getChildProperty());
            for (String value : values) {
                if (rangeValidation.visitingValue(value) == false)
                    return false;
            }
        }

        return true;
    }

    /**
     * API to validate an array of {@link SimulatorResourceModel} values.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(SimulatorResourceModel[] value) {
        return false;
    }

    /**
     * API to validate a 2D array of integer values.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(Integer[][] values) {
        if (mProperty == null)
            return false;

        if (!checkRange(values.length))
            return false;

        if (mProperty.getChildProperty() != null) {
            AttributeValueValidation rangeValidation = new AttributeValueValidation(
                    mProperty.getChildProperty());
            for (Integer[] value : values) {
                if (rangeValidation.visitingValue(value) == false)
                    return false;
            }
        }

        return true;
    }

    /**
     * API to validate a 2D array of double values.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(Double[][] values) {
        if (mProperty == null)
            return false;

        if (!checkRange(values.length))
            return false;

        if (mProperty.getChildProperty() != null) {
            AttributeValueValidation rangeValidation = new AttributeValueValidation(
                    mProperty.getChildProperty());
            for (Double[] value : values) {
                if (rangeValidation.visitingValue(value) == false)
                    return false;
            }
        }

        return true;
    }

    /**
     * API to validate a 2D array of boolean values.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(Boolean[][] values) {
        if (mProperty == null)
            return false;

        if (!checkRange(values.length))
            return false;

        if (mProperty.getChildProperty() != null) {
            AttributeValueValidation rangeValidation = new AttributeValueValidation(
                    mProperty.getChildProperty());
            for (Boolean[] value : values) {
                if (rangeValidation.visitingValue(value) == false)
                    return false;
            }
        }

        return true;
    }

    /**
     * API to validate a 2D array of string values.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(String[][] values) {
        if (mProperty == null)
            return false;

        if (!checkRange(values.length))
            return false;

        if (mProperty.getChildProperty() != null) {
            AttributeValueValidation rangeValidation = new AttributeValueValidation(
                    mProperty.getChildProperty());
            for (String[] value : values) {
                if (rangeValidation.visitingValue(value) == false)
                    return false;
            }
        }

        return true;
    }

    /**
     * API to validate a 2D array of {@link SimulatorResourceModel} values.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(SimulatorResourceModel[][] value) {
        return false;
    }

    /**
     * API to validate a 3D array of integer values.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(Integer[][][] values) {
        if (mProperty == null)
            return false;

        if (!checkRange(values.length))
            return false;

        if (mProperty.getChildProperty() != null) {
            AttributeValueValidation rangeValidation = new AttributeValueValidation(
                    mProperty.getChildProperty());
            for (Integer[][] value : values) {
                if (rangeValidation.visitingValue(value) == false)
                    return false;
            }
        }

        return true;
    }

    /**
     * API to validate a 3D array of double values.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(Double[][][] values) {
        if (mProperty == null)
            return false;

        if (!checkRange(values.length))
            return false;

        if (mProperty.getChildProperty() != null) {
            AttributeValueValidation rangeValidation = new AttributeValueValidation(
                    mProperty.getChildProperty());
            for (Double[][] value : values) {
                if (rangeValidation.visitingValue(value) == false)
                    return false;
            }
        }

        return true;
    }

    /**
     * API to validate a 3D array of boolean values.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(Boolean[][][] values) {
        if (mProperty == null)
            return false;

        if (!checkRange(values.length))
            return false;

        if (mProperty.getChildProperty() != null) {
            AttributeValueValidation rangeValidation = new AttributeValueValidation(
                    mProperty.getChildProperty());
            for (Boolean[][] value : values) {
                if (rangeValidation.visitingValue(value) == false)
                    return false;
            }
        }

        return true;
    }

    /**
     * API to validate a 3D array of string values.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(String[][][] values) {
        if (mProperty == null)
            return false;

        if (!checkRange(values.length))
            return false;

        if (mProperty.getChildProperty() != null) {
            AttributeValueValidation rangeValidation = new AttributeValueValidation(
                    mProperty.getChildProperty());
            for (String[][] value : values) {
                if (rangeValidation.visitingValue(value) == false)
                    return false;
            }
        }

        return true;
    }

    /**
     * API to validate a 3D array of {@link SimulatorResourceModel} values.
     *
     * @return True if the given value is valid, otherwise false.
     */
    @Override
    public Boolean visitingValue(SimulatorResourceModel[][][] value) {
        return false;
    }

    private boolean checkRange(double value) {
        if (AttributeProperty.Type.RANGE == mProperty.type()
                && (value >= mProperty.min() && value <= mProperty.max())) {
            return true;
        }

        return false;
    }

    private <T> boolean checkValueSet(T value) {
        if (AttributeProperty.Type.VALUESET == mProperty.type()
                && null != mProperty.valueSet()) {
            for (AttributeValue allowedValue : mProperty.valueSet()) {
                if (allowedValue.get().equals(value))
                    return true;
            }
        }

        return false;
    }
}
