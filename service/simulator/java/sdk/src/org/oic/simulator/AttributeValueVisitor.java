package org.oic.simulator;

public class AttributeValueVisitor {
    private AttributeValue mValue;
    private VisitingMethods mListener;

    public interface VisitingMethods<T extends Object> {
        public T visitingValue(Integer value);

        public T visitingValue(Double value);

        public T visitingValue(Boolean value);

        public T visitingValue(String value);

        public T visitingValue(SimulatorResourceModel value);

        public T visitingValue(Integer[] value);

        public T visitingValue(Double[] value);

        public T visitingValue(Boolean[] value);

        public T visitingValue(String[] value);

        public T visitingValue(SimulatorResourceModel[] value);

        public T visitingValue(Integer[][] value);

        public T visitingValue(Double[][] value);

        public T visitingValue(Boolean[][] value);

        public T visitingValue(String[][] value);

        public T visitingValue(SimulatorResourceModel[][] value);

        public T visitingValue(Integer[][][] value);

        public T visitingValue(Double[][][] value);

        public T visitingValue(Boolean[][][] value);

        public T visitingValue(String[][][] value);

        public T visitingValue(SimulatorResourceModel[][][] value);
    }

    public AttributeValueVisitor(AttributeValue value, VisitingMethods listener) {
        mValue = value;
        mListener = listener;
    }

    public Object visit() {
        if (null == mValue || null == mListener)
            return null;

        AttributeValue.TypeInfo typeInfo = mValue.typeInfo();
        if (AttributeValue.ValueType.INTEGER == typeInfo.mBaseType) {
            if (AttributeValue.ValueType.INTEGER == typeInfo.mType)
                return mListener.visitingValue((Integer) mValue.get());
            else if (AttributeValue.ValueType.ARRAY == typeInfo.mType) {
                if (1 == typeInfo.mDepth)
                    return mListener.visitingValue((Integer[]) mValue.get());
                if (2 == typeInfo.mDepth)
                    return mListener.visitingValue((Integer[][]) mValue.get());
                if (3 == typeInfo.mDepth)
                    return mListener.visitingValue((Integer[][][]) mValue.get());
            }
        } else if (AttributeValue.ValueType.DOUBLE == typeInfo.mBaseType) {
            if (AttributeValue.ValueType.DOUBLE == typeInfo.mType)
                return mListener.visitingValue((Double) mValue.get());
            else if (AttributeValue.ValueType.ARRAY == typeInfo.mType) {
                if (1 == typeInfo.mDepth)
                    return mListener.visitingValue((Double[]) mValue.get());
                if (2 == typeInfo.mDepth)
                    return mListener.visitingValue((Double[][]) mValue.get());
                if (3 == typeInfo.mDepth)
                    return mListener.visitingValue((Double[][][]) mValue.get());
            }
        } else if (AttributeValue.ValueType.BOOLEAN == typeInfo.mBaseType) {
            if (AttributeValue.ValueType.BOOLEAN == typeInfo.mType)
                return mListener.visitingValue((Boolean) mValue.get());
            else if (AttributeValue.ValueType.ARRAY == typeInfo.mType) {
                if (1 == typeInfo.mDepth)
                    return mListener.visitingValue((Boolean[]) mValue.get());
                if (2 == typeInfo.mDepth)
                    return mListener.visitingValue((Boolean[][]) mValue.get());
                if (3 == typeInfo.mDepth)
                    return mListener.visitingValue((Boolean[][][]) mValue.get());
            }
        } else if (AttributeValue.ValueType.STRING == typeInfo.mBaseType) {
            if (AttributeValue.ValueType.STRING == typeInfo.mType)
                return mListener.visitingValue((String) mValue.get());
            else if (AttributeValue.ValueType.ARRAY == typeInfo.mType) {
                if (1 == typeInfo.mDepth)
                    return mListener.visitingValue((String[]) mValue.get());
                if (2 == typeInfo.mDepth)
                    return mListener.visitingValue((String[][]) mValue.get());
                if (3 == typeInfo.mDepth)
                    return mListener.visitingValue((String[][][]) mValue.get());
            }
        } else if (AttributeValue.ValueType.RESOURCEMODEL == typeInfo.mBaseType) {
            if (AttributeValue.ValueType.RESOURCEMODEL == typeInfo.mType)
                return mListener.visitingValue((SimulatorResourceModel) mValue.get());
            else if (AttributeValue.ValueType.ARRAY == typeInfo.mType) {
                if (1 == typeInfo.mDepth)
                    return mListener.visitingValue((SimulatorResourceModel[]) mValue.get());
                if (2 == typeInfo.mDepth)
                    return mListener.visitingValue((SimulatorResourceModel[][]) mValue.get());
                if (3 == typeInfo.mDepth)
                    return mListener.visitingValue((SimulatorResourceModel[][][]) mValue.get());
            }
        }

        return null;
    }
}
