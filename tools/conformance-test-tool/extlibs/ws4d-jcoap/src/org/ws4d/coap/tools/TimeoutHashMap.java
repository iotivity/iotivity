package org.ws4d.coap.tools;
import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

public class TimeoutHashMap<K, V> extends HashMap<Object, Object>{

	private static final long serialVersionUID = 4987370276778256858L;

	/* chronological list to remove expired elements when update() is called */ 
	LinkedList<TimoutType<K>> timeoutQueue = new LinkedList<TimoutType<K>>();
	
	/* Default Timeout is one minute */
	long timeout = 60000;
	
	public TimeoutHashMap(long timeout){
		this.timeout = timeout;
	}
	
	@Override
	public Object put(Object key, Object value) {
		long expires = System.currentTimeMillis() + timeout;
		TimoutType<V> timeoutValue = new TimoutType<V>((V) value, expires);
		TimoutType<K> timeoutKey = new TimoutType<K>((K) key, expires);
		timeoutQueue.add(timeoutKey);
		timeoutValue = (TimoutType<V>) super.put((K) key, timeoutValue);
		if (timeoutValue != null){
			return timeoutValue.object;
		}
		return null;
	}




	@Override
	public Object get(Object key) {
		TimoutType<V> timeoutValue = (TimoutType<V>) super.get(key);	
		if (timeoutValueIsValid(timeoutValue)){
			return timeoutValue.object;
		} 
		return null;
	}	
	
	@Override
	public Object remove(Object key) {
		TimoutType<V> timeoutValue = (TimoutType<V>) super.remove(key);
		if (timeoutValueIsValid(timeoutValue)){
			return timeoutValue.object;
		} 		
		return null;
	}
	
	@Override
	public void clear() {
		super.clear();
		timeoutQueue.clear();
	}
	
	/* remove expired elements */
	public void update(){
        while(true) {
        	TimoutType<K> timeoutKey = timeoutQueue.peek();
        	if (timeoutKey == null){
        		/* if the timeoutKey queue is empty, there must be no more elements in the hashmap 
        		 * otherwise there is a bug in the implementation */
        		if (!super.isEmpty()){
        			throw new IllegalStateException("Error in TimeoutHashMap. Timeout queue is empty but hashmap not!");
        		}
        		return;
        	}
        	
        	long now = System.currentTimeMillis();
        	if (now > timeoutKey.expires){
        		timeoutQueue.poll();
        		TimoutType<V> timeoutValue = (TimoutType<V>) super.remove(timeoutKey.object);

        		if (timeoutValueIsValid(timeoutValue)){
        			/* This is a very special case which happens if an entry is overridden:
        			 * - put V with K 
        			 * - put V2 with K
        			 * - K is expired but V2 not 
        			 * because this is expected to be happened very seldom, we "reput" V2 to the hashmap 
        			 * wich is better than every time to making a get and than a remove */
        			super.put(timeoutKey.object, timeoutValue);
        		}
        	} else {
        		/* Key is not expired -> break the loop */
        		break;
        	}
        }
	}
	
	@Override
	public Object clone() {
		// TODO implement function
		throw new IllegalStateException(); 
//		return super.clone();
	}

	@Override
	public boolean containsKey(Object arg0) {
		// TODO implement function
		throw new IllegalStateException(); 
//		return super.containsKey(arg0);
	}

	@Override
	public boolean containsValue(Object arg0) {
		// TODO implement function
		throw new IllegalStateException(); 
//		return super.containsValue(arg0);
	}

	@Override
	public Set<Entry<Object, Object>> entrySet() {
		// TODO implement function
		throw new IllegalStateException(); 
//		return super.entrySet();
	}

	@Override
	public boolean isEmpty() {
		// TODO implement function
		throw new IllegalStateException(); 
//		return super.isEmpty();
	}

	@Override
	public Set<Object> keySet() {
		// TODO implement function
		throw new IllegalStateException(); 
//		return super.keySet();
	}

	@Override
	public void putAll(Map<? extends Object, ? extends Object> arg0) {
		// TODO implement function
		throw new IllegalStateException(); 
//		super.putAll(arg0);
	}

	@Override
	public int size() {
		// TODO implement function
		throw new IllegalStateException(); 
//		return super.size();
	}

	@Override
	public Collection<Object> values() {
		// TODO implement function
		throw new IllegalStateException(); 
//		return super.values();
	}

	
	/* private classes and methods */
	
	private boolean timeoutValueIsValid(TimoutType<V> timeoutValue){
		return timeoutValue != null && System.currentTimeMillis() < timeoutValue.expires;		
	}

	private class TimoutType<T>{
		public T object;
		public long expires;
		
		public TimoutType(T object, long expires) {
			super();
			this.object = object;
			this.expires = expires;
		}
	}
}
