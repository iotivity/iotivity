package org.oic.simulator.test;

import org.oic.simulator.AttributeProperty;
import org.oic.simulator.AttributeValue;
import org.oic.simulator.AttributeValueValidation;

import junit.framework.TestCase;

public class AttributeValueValidationTest extends TestCase {
    protected void setUp() throws Exception {
        super.setUp();
    }

    protected void tearDown() throws Exception {
        super.tearDown();
    }

    public void testValidateInt_P01() {
        AttributeProperty property = new AttributeProperty(1, 10);
        AttributeValueValidation validation = new AttributeValueValidation(
                property);
        assertTrue(validation.validate(new AttributeValue(5)));
    }

    public void testValidateInt_P02() {
        int[] valueSet = { 1, 5, 7 };
        AttributeProperty property = new AttributeProperty(valueSet);
        AttributeValueValidation validation = new AttributeValueValidation(
                property);
        assertTrue(validation.validate(new AttributeValue(5)));
    }

    public void testValidateInt_N01() {
        AttributeProperty property = new AttributeProperty(1, 10);
        AttributeValueValidation validation = new AttributeValueValidation(
                property);
        assertFalse(validation.validate(new AttributeValue(15)));
    }

    public void testValidateInt_N02() {
        int[] valueSet = { 1, 5, 7 };
        AttributeProperty property = new AttributeProperty(valueSet);
        AttributeValueValidation validation = new AttributeValueValidation(
                property);
        assertFalse(validation.validate(new AttributeValue(6)));
    }

    public void testValidateInt_N03() {
        AttributeValueValidation validation = new AttributeValueValidation(null);
        assertFalse(validation.validate(new AttributeValue(6)));
    }

    public void testValidateInt1DArray_P01() {
        AttributeProperty arrayProp = new AttributeProperty(1, 2);
        arrayProp.setChildProperty(new AttributeProperty(1, 10));

        int[] value = { 1, 5 };
        AttributeValueValidation validation = new AttributeValueValidation(
                arrayProp);
        assertTrue(validation.validate(new AttributeValue(value)));
    }

    public void testValidateInt1DArray_N01() {
        AttributeProperty arrayProp = new AttributeProperty(1, 2);
        arrayProp.setChildProperty(new AttributeProperty(1, 10));

        int[] value = { 1, 5, 7 };
        AttributeValueValidation validation = new AttributeValueValidation(
                arrayProp);
        assertFalse(validation.validate(new AttributeValue(value)));
    }

    public void testValidateInt1DArray_N02() {
        AttributeProperty arrayProp = new AttributeProperty(1, 2);
        arrayProp.setChildProperty(new AttributeProperty(1, 10));

        int[] value = { 1, 15 };
        AttributeValueValidation validation = new AttributeValueValidation(
                arrayProp);
        assertFalse(validation.validate(new AttributeValue(value)));
    }

    public void testValidateInt2DArray_P01() {
        AttributeProperty array1Prop = new AttributeProperty(1, 1);
        AttributeProperty array2Prop = new AttributeProperty(1, 1);
        AttributeProperty valueProp = new AttributeProperty(1, 10);
        array1Prop.setChildProperty(array2Prop);
        array2Prop.setChildProperty(valueProp);

        int[][] value = { { 1 } };
        AttributeValueValidation validation = new AttributeValueValidation(
                array1Prop);
        assertTrue(validation.validate(new AttributeValue(value)));
    }

    public void testValidateInt2DArray_N01() {
        AttributeProperty array1Prop = new AttributeProperty(1, 1);
        AttributeProperty array2Prop = new AttributeProperty(1, 1);
        AttributeProperty valueProp = new AttributeProperty(1, 10);
        array1Prop.setChildProperty(array2Prop);
        array2Prop.setChildProperty(valueProp);

        int[][] value = { { 1 }, { 2 } };
        AttributeValueValidation validation = new AttributeValueValidation(
                array1Prop);
        assertFalse(validation.validate(new AttributeValue(value)));
    }

    public void testValidateInt2DArray_N02() {
        AttributeProperty array1Prop = new AttributeProperty(1, 1);
        AttributeProperty array2Prop = new AttributeProperty(1, 1);
        AttributeProperty valueProp = new AttributeProperty(1, 10);
        array1Prop.setChildProperty(array2Prop);
        array2Prop.setChildProperty(valueProp);

        int[][] value = { { 1, 2 } };
        AttributeValueValidation validation = new AttributeValueValidation(
                array1Prop);
        assertFalse(validation.validate(new AttributeValue(value)));
    }

    public void testValidateInt2DArray_N03() {
        AttributeProperty array1Prop = new AttributeProperty(1, 1);
        AttributeProperty array2Prop = new AttributeProperty(1, 1);
        AttributeProperty valueProp = new AttributeProperty(1, 10);
        array1Prop.setChildProperty(array2Prop);
        array2Prop.setChildProperty(valueProp);

        int[][] value = { { 15 } };
        AttributeValueValidation validation = new AttributeValueValidation(
                array1Prop);
        assertFalse(validation.validate(new AttributeValue(value)));
    }

    public void testValidateDouble_P01() {
        AttributeProperty property = new AttributeProperty(1, 10);
        AttributeValueValidation validation = new AttributeValueValidation(
                property);
        assertTrue(validation.validate(new AttributeValue(5.12)));
    }

    public void testValidateDouble_P02() {
        double[] valueSet = { 1.0, 5.12, 7.0 };
        AttributeProperty property = new AttributeProperty(valueSet);
        AttributeValueValidation validation = new AttributeValueValidation(
                property);
        assertTrue(validation.validate(new AttributeValue(5.12)));
    }

    public void testValidateDouble_N01() {
        AttributeProperty property = new AttributeProperty(1, 10);
        AttributeValueValidation validation = new AttributeValueValidation(
                property);
        assertFalse(validation.validate(new AttributeValue(15.23)));
    }

    public void testValidateDouble_N02() {
        double[] valueSet = { 1.0, 5.12, 7.0 };
        AttributeProperty property = new AttributeProperty(valueSet);
        AttributeValueValidation validation = new AttributeValueValidation(
                property);
        assertFalse(validation.validate(new AttributeValue(6.0)));
    }

    public void testValidateDouble_N03() {
        AttributeValueValidation validation = new AttributeValueValidation(null);
        assertFalse(validation.validate(new AttributeValue(6.0)));
    }

    public void testValidateDouble1DArray_P01() {
        AttributeProperty arrayProp = new AttributeProperty(1, 2);
        arrayProp.setChildProperty(new AttributeProperty(1, 10));

        double[] value = { 1.2, 5.2 };
        AttributeValueValidation validation = new AttributeValueValidation(
                arrayProp);
        assertTrue(validation.validate(new AttributeValue(value)));
    }

    public void testValidateDouble1DArray_N01() {
        AttributeProperty arrayProp = new AttributeProperty(1, 2);
        arrayProp.setChildProperty(new AttributeProperty(1, 10));

        double[] value = { 1.2, 5.2, 7.2 };
        AttributeValueValidation validation = new AttributeValueValidation(
                arrayProp);
        assertFalse(validation.validate(new AttributeValue(value)));
    }

    public void testValidateDouble1DArray_N02() {
        AttributeProperty arrayProp = new AttributeProperty(1, 2);
        arrayProp.setChildProperty(new AttributeProperty(1, 10));

        double[] value = { 1.2, 15.2 };
        AttributeValueValidation validation = new AttributeValueValidation(
                arrayProp);
        assertFalse(validation.validate(new AttributeValue(value)));
    }

    public void testValidateDouble2DArray_P01() {
        AttributeProperty array1Prop = new AttributeProperty(1, 1);
        AttributeProperty array2Prop = new AttributeProperty(1, 1);
        AttributeProperty valueProp = new AttributeProperty(1, 10);
        array1Prop.setChildProperty(array2Prop);
        array2Prop.setChildProperty(valueProp);

        double[][] value = { { 1.2 } };
        AttributeValueValidation validation = new AttributeValueValidation(
                array1Prop);
        assertTrue(validation.validate(new AttributeValue(value)));
    }

    public void testValidateDouble2DArray_N01() {
        AttributeProperty array1Prop = new AttributeProperty(1, 1);
        AttributeProperty array2Prop = new AttributeProperty(1, 1);
        AttributeProperty valueProp = new AttributeProperty(1, 10);
        array1Prop.setChildProperty(array2Prop);
        array2Prop.setChildProperty(valueProp);

        double[][] value = { { 1.2 }, { 2.2 } };
        AttributeValueValidation validation = new AttributeValueValidation(
                array1Prop);
        assertFalse(validation.validate(new AttributeValue(value)));
    }

    public void testValidateDouble2DArray_N02() {
        AttributeProperty array1Prop = new AttributeProperty(1, 1);
        AttributeProperty array2Prop = new AttributeProperty(1, 1);
        AttributeProperty valueProp = new AttributeProperty(1, 10);
        array1Prop.setChildProperty(array2Prop);
        array2Prop.setChildProperty(valueProp);

        double[][] value = { { 1.2, 2.2 } };
        AttributeValueValidation validation = new AttributeValueValidation(
                array1Prop);
        assertFalse(validation.validate(new AttributeValue(value)));
    }

    public void testValidateDouble2DArray_N03() {
        AttributeProperty array1Prop = new AttributeProperty(1, 1);
        AttributeProperty array2Prop = new AttributeProperty(1, 1);
        AttributeProperty valueProp = new AttributeProperty(1, 10);
        array1Prop.setChildProperty(array2Prop);
        array2Prop.setChildProperty(valueProp);

        double[][] value = { { 15.2 } };
        AttributeValueValidation validation = new AttributeValueValidation(
                array1Prop);
        assertFalse(validation.validate(new AttributeValue(value)));
    }
}
