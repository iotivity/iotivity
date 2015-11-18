package oic.simulator.serviceprovider.utils;

import java.util.Map;

import org.oic.simulator.AttributeValue;
import org.oic.simulator.AttributeValueVisitor;
import org.oic.simulator.SimulatorResourceAttribute;
import org.oic.simulator.SimulatorResourceModel;

public class AttributeValueStringConverter implements
        AttributeValueVisitor.VisitingMethods<String> {

    private AttributeValue mValue;

    public AttributeValueStringConverter(AttributeValue value) {
        mValue = value;
    }

    @Override
    public String toString() {
        AttributeValueVisitor visitor = new AttributeValueVisitor(mValue, this);
        return (String) visitor.visit();
    }

    @Override
    public String visitingValue(Integer value) {
        return value.toString();
    }

    @Override
    public String visitingValue(Double value) {
        return value.toString();
    }

    @Override
    public String visitingValue(Boolean value) {
        return value.toString();
    }

    @Override
    public String visitingValue(String value) {
        return new String(value);
    }

    @Override
    public String visitingValue(SimulatorResourceModel value) {
        StringBuilder result = new StringBuilder();
        result.append("{");
        boolean first = true;
        for (Map.Entry<String, SimulatorResourceAttribute> entry : value
                .getAttributes().entrySet()) {
            if (!first)
                result.append(", ");
            first = false;
            result.append("\"" + entry.getKey() + "\":");
            AttributeValue attributeValue = entry.getValue().value();
            result.append(new AttributeValueStringConverter(attributeValue)
                    .toString());
        }

        result.append("}");
        return result.toString();
    }

    @Override
    public String visitingValue(Integer[] values) {
        StringBuilder result = new StringBuilder();
        result.append("[");
        boolean first = true;
        for (Integer value : values) {
            if (!first)
                result.append(", ");
            first = false;
            result.append(value.toString());
        }

        result.append("]");
        return result.toString();
    }

    @Override
    public String visitingValue(Double[] values) {
        StringBuilder result = new StringBuilder();
        result.append("[");
        boolean first = true;
        for (Double value : values) {
            if (!first)
                result.append(", ");
            first = false;
            result.append(value.toString());
        }

        result.append("]");
        return result.toString();
    }

    @Override
    public String visitingValue(Boolean[] values) {
        StringBuilder result = new StringBuilder();
        result.append("[");
        boolean first = true;
        for (Boolean value : values) {
            if (!first)
                result.append(", ");
            first = false;
            result.append(value.toString());
        }

        result.append("]");
        return result.toString();
    }

    @Override
    public String visitingValue(String[] values) {
        StringBuilder result = new StringBuilder();
        result.append("[");
        boolean first = true;
        for (String value : values) {
            if (!first)
                result.append(", ");
            first = false;
            result.append(value);
        }

        result.append("]");
        return result.toString();
    }

    @Override
    public String visitingValue(SimulatorResourceModel[] values) {
        StringBuilder result = new StringBuilder();
        result.append("[");
        boolean first = true;
        for (SimulatorResourceModel value : values) {
            if (!first)
                result.append(", ");
            first = false;
            AttributeValue attributeValue = new AttributeValue(value);
            result.append(new AttributeValueStringConverter(attributeValue)
                    .toString());
        }

        result.append("]");
        return result.toString();
    }

    @Override
    public String visitingValue(Integer[][] values) {
        StringBuilder result = new StringBuilder();
        result.append("[");
        boolean first = true;
        for (Integer[] value : values) {
            if (!first)
                result.append(", ");
            first = false;
            AttributeValue attributeValue = new AttributeValue(value);
            result.append(new AttributeValueStringConverter(attributeValue)
                    .toString());
        }

        result.append("]");
        return result.toString();
    }

    @Override
    public String visitingValue(Double[][] values) {
        StringBuilder result = new StringBuilder();
        result.append("[");
        boolean first = true;
        for (Double[] value : values) {
            if (!first)
                result.append(", ");
            first = false;
            AttributeValue attributeValue = new AttributeValue(value);
            result.append(new AttributeValueStringConverter(attributeValue)
                    .toString());
        }

        result.append("]");
        return result.toString();
    }

    @Override
    public String visitingValue(Boolean[][] values) {
        StringBuilder result = new StringBuilder();
        result.append("[");
        boolean first = true;
        for (Boolean[] value : values) {
            if (!first)
                result.append(", ");
            first = false;
            AttributeValue attributeValue = new AttributeValue(value);
            result.append(new AttributeValueStringConverter(attributeValue)
                    .toString());
        }

        result.append("]");
        return result.toString();
    }

    @Override
    public String visitingValue(String[][] values) {
        StringBuilder result = new StringBuilder();
        result.append("[");
        boolean first = true;
        for (String[] value : values) {
            if (!first)
                result.append(", ");
            first = false;
            AttributeValue attributeValue = new AttributeValue(value);
            result.append(new AttributeValueStringConverter(attributeValue)
                    .toString());
        }

        result.append("]");
        return result.toString();
    }

    @Override
    public String visitingValue(SimulatorResourceModel[][] values) {
        StringBuilder result = new StringBuilder();
        result.append("[");
        boolean first = true;
        for (SimulatorResourceModel[] value : values) {
            if (!first)
                result.append(", ");
            first = false;
            AttributeValue attributeValue = new AttributeValue(value);
            result.append(new AttributeValueStringConverter(attributeValue)
                    .toString());
        }

        result.append("]");
        return result.toString();
    }

    @Override
    public String visitingValue(Integer[][][] values) {
        StringBuilder result = new StringBuilder();
        result.append("[");
        boolean first = true;
        for (Integer[][] value : values) {
            if (!first)
                result.append(", ");
            first = false;
            AttributeValue attributeValue = new AttributeValue(value);
            result.append(new AttributeValueStringConverter(attributeValue)
                    .toString());
        }

        result.append("]");
        return result.toString();
    }

    @Override
    public String visitingValue(Double[][][] values) {
        StringBuilder result = new StringBuilder();
        result.append("[");
        boolean first = true;
        for (Double[][] value : values) {
            if (!first)
                result.append(", ");
            first = false;
            AttributeValue attributeValue = new AttributeValue(value);
            result.append(new AttributeValueStringConverter(attributeValue)
                    .toString());
        }

        result.append("]");
        return result.toString();
    }

    @Override
    public String visitingValue(Boolean[][][] values) {
        StringBuilder result = new StringBuilder();
        result.append("[");
        boolean first = true;
        for (Boolean[][] value : values) {
            if (!first)
                result.append(", ");
            first = false;
            AttributeValue attributeValue = new AttributeValue(value);
            result.append(new AttributeValueStringConverter(attributeValue)
                    .toString());
        }

        result.append("]");
        return result.toString();
    }

    @Override
    public String visitingValue(String[][][] values) {
        StringBuilder result = new StringBuilder();
        result.append("[");
        boolean first = true;
        for (String[][] value : values) {
            if (!first)
                result.append(", ");
            first = false;
            AttributeValue attributeValue = new AttributeValue(value);
            result.append(new AttributeValueStringConverter(attributeValue)
                    .toString());
        }

        result.append("]");
        return result.toString();
    }

    @Override
    public String visitingValue(SimulatorResourceModel[][][] values) {
        StringBuilder result = new StringBuilder();
        result.append("[");
        boolean first = true;
        for (SimulatorResourceModel[][] value : values) {
            if (!first)
                result.append(", ");
            first = false;
            AttributeValue attributeValue = new AttributeValue(value);
            result.append(new AttributeValueStringConverter(attributeValue)
                    .toString());
        }

        result.append("]");
        return result.toString();
    }

}
