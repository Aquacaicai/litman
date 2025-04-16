let config = {
    apiRoot: process.env.NODE_ENV === 'development' ? 'http://127.0.0.1:5000' : window.API_ROOT,
    timeout: 15000,
    headers: {
        'Content-Type': 'application/json'
    }
};

export function setApiRoot(newApiRoot) {
    config.apiRoot = newApiRoot;
    console.log(`API root changed to: ${newApiRoot}`);
}

export default config;
