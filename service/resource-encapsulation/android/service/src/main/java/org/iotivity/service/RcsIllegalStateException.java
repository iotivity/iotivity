package org.iotivity.service;

/**
 * Thrown when an action is attempted at a time when not in the correct state.
 *
 */
public class RcsIllegalStateException extends RcsException {

    private static final long serialVersionUID = 6142669404885957616L;

    public RcsIllegalStateException(String message) {
        super(message);
    }

}
