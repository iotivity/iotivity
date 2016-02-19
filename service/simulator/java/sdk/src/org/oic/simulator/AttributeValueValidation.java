package org.oic.simulator;

public class AttributeValueValidation implements
        AttributeValueVisitor.VisitingMethods<Boolean> {
    private AttributeProperty mProperty = null;

    public AttributeValueValidation(AttributeProperty property) {
        mProperty = property;
    }

    public boolean validate(AttributeValue value) {
        AttributeValueVisitor visitor = new AttributeValueVisitor(value, this);
        Boolean result = (Boolean) visitor.visit();
        return result.booleanValue();
    }

    @Override
    public Boolean visitingValue(Integer value) {
        if (mProperty == null)
            return false;

        if (checkRange(value.doubleValue()) || checkValueSet(value))
            return true;
        return false;
    }

    @Override
    public Boolean visitingValue(Double value) {
        if (mProperty == null)
            return false;

        if (checkRange(value.doubleValue()) || checkValueSet(value))
            return true;
        return false;
    }

    @Override
    public Boolean visitingValue(Boolean value) {
        if (mProperty == null)
            return false;

        if (checkValueSet(value))
            return true;
        return false;
    }

    @Override
    public Boolean visitingValue(String value) {
        if (mProperty == null)
            return false;

        if (checkRange(value.length()) || checkValueSet(value))
            return true;
        return false;
    }

    @Override
    public Boolean visitingValue(SimulatorResourceModel value) {
        return false;
    }

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

    @Override
    public Boolean visitingValue(SimulatorResourceModel[] value) {
        return false;
    }

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

    @Override
    public Boolean visitingValue(SimulatorResourceModel[][] value) {
        return false;
    }

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
